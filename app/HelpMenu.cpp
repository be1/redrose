#include "HelpMenu.h"
#include "config.h"
#include <QMessageBox>
#include "AbcApplication.h"
#include "logwindow.h"

HelpMenu::HelpMenu(QWidget* parent)
	: QMenu(parent)
{
    setTitle(tr("Help"));
    addAction(tr("View Log window"), this, &HelpMenu::onViewLogActionTriggered);
    addAction(tr("About"), this, &HelpMenu::onAboutActionTriggered);
    addAction(tr("About Qt"), this, &HelpMenu::onAboutQtActionTriggered);
}

HelpMenu::~HelpMenu()
{
}

void HelpMenu::onAboutActionTriggered()
{
    AbcApplication* a = static_cast<AbcApplication*>(qApp);
    QString paragraph = tr("\n\nThank you for using Redrose. I hope you enjoy this software to write your musical scores. Redrose is free software: you can contribute to the code, design or translations if you are interested. ");
    paragraph += tr("You can send me an email to tell what kind of music you are writing with Redrose. I would be happy to know it. ");
    paragraph += tr("If you want, you can also make a small donation to me by reaching my paypal page <https://paypal.me/brouits>.\n");
    QMessageBox::about(a->mainWindow(), tr("ABC score editor"), tr("\nRedrose version ") + VERSION + " (" + REVISION + ")\n" + tr( "Copyright © 2021 Benoît Rouits <brouits@free.fr>") + paragraph);
}

void HelpMenu::onAboutQtActionTriggered()
{
    AbcApplication* a = static_cast<AbcApplication*>(qApp);
    QMessageBox::aboutQt(a->mainWindow(), tr("ABC score editor"));
}

void HelpMenu::onViewLogActionTriggered()
{
    AbcApplication* a = static_cast<AbcApplication*>(qApp);
    if (a->logWindow()) {
        a->logWindow()->hide();
        a->logWindow()->show();
    } else {
        LogWindow* window = new LogWindow;
        a->setLogWindow(window);
        window->show();
    }
}
