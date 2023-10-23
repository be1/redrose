#include <libspectre/spectre-document.h>
#include "scorepswidget.h"
#include <QPainter>
#include <QDebug>

ScorePsWidget::ScorePsWidget(QWidget *parent):
    QLabel(parent)
{
    m_render_context = spectre_render_context_new();
}

ScorePsWidget::ScorePsWidget(const QString &filename, QWidget *parent):
    QLabel(parent)
{
    m_render_context = spectre_render_context_new();
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
    spectre_render_context_set_scale(m_render_context, (double) width() / (double) w, (double) height() / (double) h);
    spectre_page_render(m_current_page, m_render_context, &m_page_data, &m_row_length);

    QImage image(m_page_data, width(), height(), m_row_length, QImage::Format_RGB32);
    setPixmap(QPixmap::fromImage(image));
}

void ScorePsWidget::load(const QString &filename)
{

    spectre_document_free(m_document);
    m_document = nullptr;

    if (filename.isEmpty()) {
        displayPage(-1); /* clear view */
        return;
    }

    m_document = spectre_document_new();
    spectre_document_load(m_document, filename.toLocal8Bit().constData());

    if (spectre_document_status(m_document) != SPECTRE_STATUS_SUCCESS) {
        spectre_document_free(m_document);
        return;
    }

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

    refresh();
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

    QImage image(page_data, pwidth, pheight, row_length, QImage::Format_RGB32);
    painter->drawImage(0, 0, image);

    spectre_page_free(page);
    spectre_render_context_free(context);
    free(page_data);
}
