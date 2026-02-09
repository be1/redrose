#include <libspectre/spectre-document.h>
#include "scorepswidget.h"
#include <QPainter>
#include <QFileInfo>
#include <QDebug>

ScorePsWidget::ScorePsWidget(QWidget* parent):
    QGraphicsView(parent)
{
    initialize();
}

void ScorePsWidget::initialize()
{
    viewport()->installEventFilter(this);
    setMouseTracking(true);

    m_scene = new QGraphicsScene(this);
    setScene(m_scene);

    m_render_context = spectre_render_context_new();

    m_refreshTimer.setInterval(50);
    m_refreshTimer.setSingleShot(true);
    connect(&m_refreshTimer, &QTimer::timeout, this, &ScorePsWidget::refresh);
}

ScorePsWidget::ScorePsWidget(const QString& filename, QWidget* parent):
    QGraphicsView(parent)
{
    initialize();
    load(filename);
}

ScorePsWidget::~ScorePsWidget()
{
    cleanup();

    if (m_current_page)
        spectre_page_free(m_current_page);

    if (m_render_context)
        spectre_render_context_free(m_render_context);

    if (m_document)
        spectre_document_free(m_document);
}

void ScorePsWidget::cleanup()
{
    free(m_page_data);
    m_page_data = nullptr;
}

void ScorePsWidget::refresh()
{
    if (!m_current_page)
        return;

    cleanup();

    int w, h;
    spectre_page_get_size(m_current_page, &w, &h);

    double item_width, item_height;
    item_width = m_default_scale_factor * width();
    item_height = item_width * h / w;

    //spectre_render_context_set_resolution(m_render_context, 75., 75.);
    spectre_render_context_set_scale(m_render_context, item_width / (double) w, item_height / (double) h);
    spectre_page_render(m_current_page, m_render_context, &m_page_data, &m_row_length);

    QImage image(m_page_data, item_width, item_height, m_row_length, QImage::Format_RGBX8888);

    setPixmap(QPixmap::fromImage(image));
    m_scene->setSceneRect(0, 0, item_width, item_height);

    if (!m_window_resizing) {
        zoomReset();
    }
}

void ScorePsWidget::resizeEvent(QResizeEvent* event) {
    QGraphicsView::resizeEvent(event);
    if (event->size().width() != width() && event->size().height() != height()) {
        if (m_refreshTimer.isActive())
            m_refreshTimer.stop();
    }

    m_window_resizing = true;
    m_refreshTimer.start();
}

void ScorePsWidget::keyReleaseEvent(QKeyEvent* ev) {
    QGraphicsView::keyReleaseEvent(ev);
    if (ev->key() == Qt::Key_Control) {
        this->m_ctrl = false;
        ev->accept();
    }
}

void ScorePsWidget::keyPressEvent(QKeyEvent* ev) {
    QGraphicsView::keyPressEvent(ev);
    int key = ev->key();
    if (key == Qt::Key_Control) {
        this->m_ctrl = true;
        ev->accept();
    }

    /* do not (un)zoom if no data displayed */
    if (isEmpty())
        return;

    if (key == Qt::Key_Equal || key == Qt::Key_0 || key == Qt::Key_Plus || key == Qt::Key_Minus) {
        if (m_ctrl) {
            if (key == Qt::Key_0 || key == Qt::Key_Equal)
                zoomReset();
            else if (key == Qt::Key_Plus)
                zoomIn();
            else
                zoomOut();

            ev->accept();
        }
    }
}

void ScorePsWidget::wheelEvent(QWheelEvent* ev) {
    QPoint delta = ev->angleDelta();
    //if (!delta.isNull() && m_ctrl) {
    if (!delta.isNull() && (ev->modifiers() & Qt::ControlModifier)) {

        /* do not (un)zoom if no data displayed */
        if (isEmpty())
            return;

        /* grab future Ctrl+= or Ctrl+0  */
        setFocus();

        int way = delta.y();
        if (way == 0)
            return;

        if (way < 0) {
            zoomOut();
        } else if (way > 0) {
            zoomIn();
        }

        QPointF newPos = mapToScene(ev->position().toPoint());
        centerOn(newPos);

        ev->accept();
        return;
    }

    QGraphicsView::wheelEvent(ev);
}

void ScorePsWidget::zoomIn()
{
    if (m_scale_factor > m_max_scale_factor * (1 / m_scale_coef))
        return;

    scale(m_scale_coef, m_scale_coef);
    m_scale_factor *= m_scale_coef;
}

void ScorePsWidget::zoomOut()
{
    if (m_scale_factor <= 0.1)
        return;

    scale(1 / m_scale_coef, 1/ m_scale_coef);
    m_scale_factor /= m_scale_coef;
}

void ScorePsWidget::zoomReset()
{
    /* reset zoom to 1:1 */
    scale (1. / m_scale_factor, 1. / m_scale_factor);
    m_scale_factor = 1. / m_scale_factor;

    /* reset zoom to 1:x */
    scale (1. / m_default_scale_factor, 1. / m_default_scale_factor);
    m_scale_factor = 1. / m_default_scale_factor;
}

void ScorePsWidget::setPixmap(const QPixmap& pm)
{
    if (m_pixmap_item) {
        scene()->removeItem(m_pixmap_item);
        delete m_pixmap_item;
    }

    m_pixmap_item = new QGraphicsPixmapItem(pm);
    scene()->addItem(m_pixmap_item);
}

bool ScorePsWidget::isEmpty()
{
    return m_current_page == nullptr;
}

void ScorePsWidget::load(const QString& filename)
{
    spectre_document_free(m_document);
    m_document = nullptr;

    if (filename.isEmpty()) {
        displayPage(-1); /* clear view */
        return;
    }

    QFileInfo info(filename);
    if (!info.exists())
        return;

    m_document = spectre_document_new();

    spectre_document_load(m_document, filename.toLocal8Bit().constData());

    if (spectre_document_status(m_document) != SPECTRE_STATUS_SUCCESS) {
        spectre_document_free(m_document);
        return;
    }

    m_window_resizing = false;
    displayPage(0);
}

int ScorePsWidget::getNumberOfPages()
{
    if (!m_document)
        return 0;

    return spectre_document_get_n_pages(m_document);
}

void ScorePsWidget::displayPage(int index)
{
    if (m_current_page)
        spectre_page_free(m_current_page);
    m_current_page = nullptr;

    if (index < 0) {
        /* clear view */
        setPixmap(QPixmap());
        return;
    }

    m_current_page = spectre_document_get_page(m_document, index);

    if (m_refreshTimer.isActive())
        m_refreshTimer.stop();

    m_refreshTimer.start();
}

void ScorePsWidget::printPage(int index, QPainter* painter)
{
    SpectrePage* page = spectre_document_get_page(m_document, index);
    SpectreRenderContext* context = spectre_render_context_new();

    int w, h, pwidth, pheight;
    unsigned char* page_data = nullptr;
    int row_length;

    pwidth = painter->device()->width();
    pheight = painter->device()->height();

    spectre_page_get_size(page, &w, &h);
    spectre_render_context_set_scale(context, (double) pwidth / (double) w, (double) pheight / (double) h);
    spectre_page_render(page, context, &page_data, &row_length);

    QImage image(page_data, pwidth, pheight, row_length, QImage::Format_RGBX8888);
    painter->drawImage(0, 0, image);

    spectre_page_free(page);
    spectre_render_context_free(context);
    free(page_data);
}
