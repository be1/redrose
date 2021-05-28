#include "HelpMenu.h"
#include "config.h"
#include <QMessageBox>
#include "AbcApplication.h"

HelpMenu::HelpMenu(QWidget* parent)
	: QMenu(parent)
{
    setTitle(tr("Help"));
    aboutaction.setText(tr("About"));
	addAction(&aboutaction);
    connect(&aboutaction, SIGNAL(triggered()), this, SLOT(onAboutActionTriggered()));

    aboutqtaction.setText(tr("About Qt"));
	addAction(&aboutqtaction);
	connect(&aboutqtaction, SIGNAL(triggered()), this, SLOT(onAboutQtActionTriggered()));
}

HelpMenu::~HelpMenu()
{
}

void HelpMenu::onAboutActionTriggered()
{
    AbcApplication* a = static_cast<AbcApplication*>(qApp);
    QMessageBox::about(a->mainWindow(), tr("ABC score editor"), tr("\nRedrose version ") + VERSION + " (" + REVISION + ")\n" +tr( "Copyright © 2021 Benoît Rouits <brouits@free.fr>"));
}

void HelpMenu::onAboutQtActionTriggered()
{
    AbcApplication* a = static_cast<AbcApplication*>(qApp);
    QMessageBox::aboutQt(a->mainWindow(), tr("ABC score editor"));
}
