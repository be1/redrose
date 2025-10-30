#ifndef VIEWVBOXLAYOUT_H
#define VIEWVBOXLAYOUT_H

#include "RunPushButton.h"
#include "scorepswidget.h"
#include <QSplitter>
#include <QHBoxLayout>
#include <QScrollArea>

class ViewVSplitter: public QSplitter
{
	Q_OBJECT

public:
    ViewVSplitter(QWidget* parent = nullptr);
    ~ViewVSplitter();

    ScorePsWidget* psWidget();
    QPushButton* prevButton() { return &prev; }
    QPushButton* printButton() { return &print; }
    QPushButton* nextButton() { return &next; }

    void initBasename(const QString& orig, const QString& tmpbase, const QString& tmpdir);

    long currentPage() { return currentpage; }
    long lastPage() { return lastpage; }

    bool gotoPage(int page); /* starts with 1! 0 means cleanup */
    bool turnPage(int diff);

    void cleanup();

protected:
    void resizeEvent(QResizeEvent *event) override {
        QSplitter::resizeEvent(event);

        int w, h;
        int viewportWidth = area->viewport()->size().width();
        int viewportHeight = area->viewport()->size().height();
        if (viewportWidth * 297 > viewportHeight * 210) {
            w = viewportWidth;
            h = viewportWidth * 297 / 210;
        } else {
            w = viewportHeight * 210 / 297;
            h = viewportHeight;
        }
        area->widget()->resize(w, h);
    }

protected slots:
    void prevPageClicked();
    void printClicked();
    void nextPageClicked();

private:
    QScrollArea *area;
    ScorePsWidget pswidget;
    QHBoxLayout pageslayout;
    QPushButton prev;
    QPushButton print;
    QPushButton next;

    long currentpage = 0;
    long lastpage = 0;
    QString origname;
    QString basename;
    QString basedir;
};
#endif
