#ifndef VIEWVBOXLAYOUT_H
#define VIEWVBOXLAYOUT_H

#include "RunPushButton.h"
#include "LogView.h"
#include "ScoreSvgWidget.h"
#include <QSplitter>
#include <QHBoxLayout>

class ViewVSplitter: public QSplitter
{
	Q_OBJECT

public:
    ViewVSplitter(QWidget* parent = nullptr);
    ~ViewVSplitter();

    ScoreSvgWidget* svgWidget();
    LogView *logView();

    void initBasename(const QString& b, const QString& d);
    bool requestPage(int page);
    void cleanup();

protected slots:
    void prevPageClicked();
    void printClicked();
    void nextPageClicked();

private:
    ScoreSvgWidget svgwidget;
    QHBoxLayout pageslayout;
    QPushButton prev;
    QPushButton print;
    QPushButton next;

    LogView logview;
    int currentpage = 0;
    int lastpage = 0;
    QString basename;
    QString basedir;
    QStringList svgnames;
};
#endif
