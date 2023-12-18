#ifndef ABCAPPLICATION_H
#define ABCAPPLICATION_H

#include <QApplication>

#include "AbcMainWindow.h"
#include "AbcProcess.h"
#include "logwindow.h"

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

    LogWindow *logWindow() const;
    void setLogWindow(LogWindow *newLogwindow);

public slots:
    static void quit();

private:
    AbcMainWindow *abcmainwindow;
    LogWindow *logwindow;
    static bool isquit;
};
#endif
