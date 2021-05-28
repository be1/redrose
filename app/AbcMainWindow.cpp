#include "AbcMainWindow.h"
#include <QMessageBox>

AbcMainWindow::AbcMainWindow(QWidget* parent)
	: QMainWindow(parent)
{
	setWindowTitle(tr("Redrose"));
	menuBar()->addMenu(&scoremenu);
	menuBar()->addMenu(&preferencesmenu);
    menuBar()->addMenu(&helpmenu);
    setCentralWidget(&mainhsplitter);
    statusBar()->show();
    setMinimumWidth(1280);
    setMinimumHeight(720);
    show();
}

AbcMainWindow::~AbcMainWindow()
{
}

MainHSplitter *AbcMainWindow::mainHSplitter()
{
    return &mainhsplitter;
}

void AbcMainWindow::closeEvent(QCloseEvent *event)
{
    if (QMessageBox::StandardButton::Yes == scoremenu.gracefulQuit()) {
        event->accept();
    } else {
        event->ignore();
    }
}
