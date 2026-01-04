#ifndef SCOREPSWIDGET_H
#define SCOREPSWIDGET_H

#include <QTimer>
#include <QImage>
#include <QPixmap>
#include <QGraphicsView>
#include <QGraphicsPixmapItem>
#include <QPrinter>
#include <libspectre/spectre-document.h>
#include <qevent.h>

class ScorePsWidget : public QGraphicsView
{
    Q_OBJECT
public:
    ScorePsWidget(QWidget* parent = nullptr);
    ScorePsWidget(const QString& filename, QWidget* parent = nullptr);
    ~ScorePsWidget();

    void load (const QString& filename);

    int getNumberOfPages();
    void displayPage(int index); /* starting at 0 */
    void printPage(int index, QPainter* painter);

protected slots:
    void cleanup();
    void refresh();

    void resizeEvent(QResizeEvent* event) override;;
    void keyReleaseEvent(QKeyEvent* ev) override;
    void keyPressEvent(QKeyEvent* ev) override;
    void wheelEvent(QWheelEvent* ev) override;

    void zoomIn();
    void zoomOut();
    void zoomReset();

protected:
    void setPixmap(const QPixmap& pm);
    bool isEmpty();

private:
    void initialize();
    QGraphicsScene* m_scene = nullptr;
    QGraphicsPixmapItem* m_pixmap_item = nullptr;
    SpectreDocument* m_document = nullptr;
    SpectrePage* m_current_page = nullptr;
    SpectreRenderContext* m_render_context = nullptr;
    unsigned char* m_page_data = nullptr;
    int m_row_length = 0;
    QTimer m_refreshTimer;
    bool m_ctrl = false;
    const qreal m_scale_coef = 1.1;
    qreal m_scale_factor = 1;
    qreal m_max_scale_factor = 2;
    qreal m_default_scale_factor = m_max_scale_factor;
    bool m_window_resizing = false;
};

#endif // SCOREPSWIDGET_H
