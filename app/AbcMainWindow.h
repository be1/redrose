#ifndef ABCMAINWINDOW_H
#define ABCMAINWINDOW_H

#include "HelpMenu.h"
#include "PreferencesMenu.h"
#include "ScoreMenu.h"
#include "MainHSplitter.h"
#include "navmenu.h"
#include <QStatusBar>
#include <QDockWidget>
#include <QToolBar>
#include <QMenuBar>
#include <QWidget>
#include <QMainWindow>

class AbcMainWindow: public QMainWindow
{
	Q_OBJECT

public:
	AbcMainWindow(QWidget* parent = nullptr);
    ~AbcMainWindow();

    MainHSplitter* mainHSplitter();
    ScoreMenu* scoreMenu();

protected:
    void closeEvent(QCloseEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;

private:
    ScoreMenu scoremenu;
    NavMenu navmenu;
	PreferencesMenu preferencesmenu;
    HelpMenu helpmenu;
    MainHSplitter mainhsplitter;
};
#endif
