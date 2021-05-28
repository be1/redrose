#include "AbcApplication.h"
#include <QDebug>
#include "config.h"
#include "settings.h"
#include "../abcm2ps/abcm2ps.h"
#include "settings.h"

AbcApplication::AbcApplication(int& argc, char **argv)
	: QApplication(argc, argv)
{
    setOrganizationName(SETTINGS_DOMAIN);
	setOrganizationDomain("herewe");
    setApplicationName(SETTINGS_APP);
	setApplicationVersion(VERSION " (" REVISION ")");

    Settings settings;
    settings.check();

	abcminit();
}

AbcApplication::~AbcApplication()
{
}

void AbcApplication::setMainWindow(AbcMainWindow* w)
{
	abcmainwindow = w;
}

AbcMainWindow *AbcApplication::mainWindow()
{
	return abcmainwindow;
}

void AbcApplication::openFileNames(const QStringList &fileNames)
{
        EditTabWidget* edittabs = mainWindow()->mainHSplitter()->editTabWidget();
        for (int i = 0; i < fileNames.length(); i++) {
                QString fileName = fileNames[i];
                QFile file(fileName);
                if (file.open(QFile::ReadOnly | QFile::Text)) {
                        EditWidget* widget = new EditWidget(fileName, edittabs);
                        AbcPlainTextEdit *edit = widget->editVBoxLayout()->abcPlainTextEdit();
                        edit->setPlainText(file.readAll());
                        file.close();
                        edit->setSaved();
                        edittabs->addTab(widget);
                }
        }
}

bool AbcApplication::isquit = false;

void AbcApplication::quit()
{
    AbcApplication::isquit = true;
    QApplication::quit();
}

bool AbcApplication::isQuit()
{
    return AbcApplication::isquit;
}
