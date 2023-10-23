#ifndef SCOREPSWIDGET_H
#define SCOREPSWIDGET_H

#include <QImage>
#include <QPixmap>
#include <QLabel>
#include <QPrinter>
#include <libspectre/spectre-document.h>

class ScorePsWidget : public QLabel
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

protected:
    void cleanup();
    void refresh();

    void resizeEvent(QResizeEvent* event) override {
        QLabel::resizeEvent(event);
        refresh();
    };

private:
    SpectreDocument* m_document = nullptr;
    SpectrePage* m_current_page = nullptr;
    SpectreRenderContext* m_render_context = nullptr;
    unsigned char* m_page_data = nullptr;
    int m_row_length = 0;
    QImage* m_image = nullptr;
};

#endif // SCOREPSWIDGET_H
