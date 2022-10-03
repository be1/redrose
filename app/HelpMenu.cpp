#include "HelpMenu.h"
#include "config.h"
#include <QMessageBox>
#include "AbcApplication.h"

HelpMenu::HelpMenu(QWidget* parent)
	: QMenu(parent)
{
    setTitle(tr("Help"));
    addAction(tr("About"), this, &HelpMenu::onAboutActionTriggered);
    addAction(tr("About Qt"), this, &HelpMenu::onAboutQtActionTriggered);
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
