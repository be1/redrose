#include "AbcMainWindow.h"
#include "AbcApplication.h"

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

ScoreMenu *AbcMainWindow::scoreMenu()
{
    return &scoremenu;
}

void AbcMainWindow::closeEvent(QCloseEvent *event)
{
    if (QMessageBox::StandardButton::Yes == scoremenu.gracefulQuit()) {
        AbcApplication* a = static_cast<AbcApplication*>(qApp);
        EditTabWidget* tabs = a->mainWindow()->mainHSplitter()->editTabWidget();
        tabs->removeTabs();
        a->quit();
        event->accept();
    } else {
        event->ignore();
    }
}

void AbcMainWindow::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Home) {
        QWidget* w = mainhsplitter.editTabWidget()->currentWidget();
        EditWidget* ew = static_cast<EditWidget*>(w);
        emit ew->editVBoxLayout()->positionSlider()->sliderMoved(0);
    } else {
        QMainWindow::keyPressEvent(event);
    }
}
