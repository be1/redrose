#ifndef SCORESVGWIDGET_H
#define SCORESVGWIDGET_H

#include <QSvgWidget>
#include <QPrinter>


class ScoreSvgWidget : public QSvgWidget
{
    Q_OBJECT
public:
    ScoreSvgWidget(QWidget *parent = nullptr);

    bool print(QPrinter *printer);

    //void dumpImage(const QString& outfileName);
};

#endif // SCORESVGWIDGET_H
