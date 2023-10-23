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

    void initBasename(const QString& b, const QString& d);
    bool requestPage(int page);
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

    int currentpage = 0;
    int lastpage = 0;
    QString basename;
    QString basedir;
    QStringList psnames;
};
#endif
