#include "AbcMainWindow.h"
#include "AbcApplication.h"

AbcMainWindow::AbcMainWindow(QWidget* parent)
	: QMainWindow(parent)
{
	setWindowTitle(tr("Redrose"));
    menuBar()->addMenu(&scoremenu);
    menuBar()->addMenu(&navmenu);
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
    QWidget* w = mainhsplitter.editTabWidget()->currentWidget();
    EditWidget* ew = static_cast<EditWidget*>(w);

    ViewVSplitter* vvs = mainHSplitter()->viewWidget();

    switch (event->key()) {
    case Qt::Key_Home: {
        emit ew->editVBoxLayout()->positionSlider()->sliderMoved(0);
        emit vvs->gotoPage(1);
        event->accept();
        break;
    }
    case Qt::Key_End: {
        long max = ew->editVBoxLayout()->positionSlider()->maximum();
        emit ew->editVBoxLayout()->positionSlider()->sliderMoved(max);
        vvs->gotoPage(vvs->lastPage());
        event->accept();
        break;
    }
    case Qt::Key_Plus: {
        if (vvs->currentPage() < vvs->lastPage()) {
            vvs->turnPage(1);
            event->accept();
        }
        break;
    }
    case Qt::Key_Minus: {
        if (vvs->currentPage() > 1) {
            vvs->turnPage(-1);
            event->accept();
        }
        break;
    }
    case Qt::Key_Space: {
        emit ew->editVBoxLayout()->playPushButton()->clicked();
        event->accept();
        break;
    }
    case Qt::Key_Return: {
        emit ew->editVBoxLayout()->runPushButton()->clicked();
        event->accept();
        break;
    }
    default:
        QMainWindow::keyPressEvent(event);
    }
}
