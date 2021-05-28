#ifndef ABCAPPLICATION_H
#define ABCAPPLICATION_H

#include "AbcMainWindow.h"
#include "AbcProcess.h"
#include <QApplication>

class AbcApplication: public QApplication
{
	Q_OBJECT

public:
	AbcApplication(int& argc, char **argv);
	~AbcApplication();

	void setMainWindow(AbcMainWindow* w);
	AbcMainWindow *mainWindow();
    void openFileNames(const QStringList& fileNames);
    static bool isQuit();
public slots:
    static void quit();

private:
    AbcMainWindow *abcmainwindow;
    static bool isquit;
};
#endif
