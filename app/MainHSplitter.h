#ifndef MAINHBOXLAYOUT_H
#define MAINHBOXLAYOUT_H

#include "ViewVSplitter.h"
#include "EditTabWidget.h"

class MainHSplitter: public QSplitter
{
	Q_OBJECT

public:
    MainHSplitter(QWidget* parent = nullptr);
    ~MainHSplitter();

    EditTabWidget *editTabWidget();
    ViewVSplitter *viewWidget();

private:
	EditTabWidget edittabwidget;
    ViewVSplitter viewwidget;
};
#endif
