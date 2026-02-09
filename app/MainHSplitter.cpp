#include "MainHSplitter.h"

MainHSplitter::MainHSplitter(QWidget* parent)
    : QSplitter(parent)
{
    setOrientation(Qt::Horizontal);
    addWidget(&edittabwidget);
    addWidget(&viewwidget);
    QList<int> sizes;
    sizes.append(400);
    sizes.append(400);
    setSizes(sizes);
}

MainHSplitter::~MainHSplitter()
{
}

EditTabWidget* MainHSplitter::editTabWidget()
{
    return &edittabwidget;
}

ViewVSplitter* MainHSplitter::viewWidget()
{
    return &viewwidget;
}
