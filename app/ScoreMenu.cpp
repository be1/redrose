#include "ScoreMenu.h"
#include "AbcApplication.h"
#include "AbcPlainTextEdit.h"
#include "EditTabWidget.h"
#include "EditWidget.h"
#include <QMessageBox>
#include <QApplication>
#include <QFileDialog>
#include <QStandardPaths>
#include <QDebug>

ScoreMenu::ScoreMenu(QWidget* parent)
	: QMenu(parent)
{
    setTitle(tr("Score"));

    newaction.setText(tr("New"));
    newaction.setShortcut(QKeySequence(QKeySequence::New));
    addAction(&newaction);

    openaction.setText(tr("Open"));
    openaction.setShortcut(QKeySequence(QKeySequence::Open));
    addAction(&openaction);

    saveaction.setText(tr("Save"));
    saveaction.setShortcut(QKeySequence(QKeySequence::Save));
    addAction(&saveaction);

    saveasaction.setText(tr("Save as"));
    addAction(&saveasaction);

    exportaction.setText(tr("Export to MIDI"));
    addAction(&exportaction);

    exppsaction.setText(tr("Export to Postscript"));
    addAction(&exppsaction);

    closeaction.setText(tr("Close"));
    closeaction.setShortcut(QKeySequence(QKeySequence::Close));
    addAction(&closeaction);

    quitaction.setText(tr("Quit"));
    quitaction.setShortcut(QKeySequence(QKeySequence::Quit));
	addAction(&quitaction);

	connect(&quitaction, SIGNAL(triggered()), this, SLOT(onQuitActionTriggered()));
    connect(&openaction, SIGNAL(triggered()), this, SLOT(onOpenActionTriggered()));
    connect(&saveaction, SIGNAL(triggered()), this, SLOT(onSaveActionTriggered()));
    connect(&saveasaction, SIGNAL(triggered()), this, SLOT(onSaveAsActionTriggered()));
    connect(&closeaction, SIGNAL(triggered()), this, SLOT(onCloseActionTriggered()));
    connect(&newaction, SIGNAL(triggered()), this, SLOT(onNewActionTriggered()));
    connect(&exportaction, SIGNAL(triggered()), this, SLOT(onExportActionTriggered()));
    connect(&exppsaction, SIGNAL(triggered()), this, SLOT(onExportPsActionTriggered()));
}

ScoreMenu::~ScoreMenu()
{
}

QMessageBox::StandardButton ScoreMenu::gracefulQuit()
{
    AbcApplication* a = static_cast<AbcApplication*>(qApp);
    EditTabWidget* tabs = a->mainWindow()->mainHSplitter()->editTabWidget();

    int unsaved= 0;
    for (int i = 0; i < tabs->editWidgetList()->length(); i++) {
        if (!tabs->editWidgetList()->at(i)->editVBoxLayout()->abcPlainTextEdit()->isSaved())
            unsaved++;
    }

    if (unsaved && QMessageBox::StandardButton::No == QMessageBox::question(a->mainWindow(), tr("Really quit?"),
                                                                            QString::number(unsaved) +
                                                                            tr(" score(s) not saved.\nDo you want to quit anyway?")))
        return QMessageBox::StandardButton::No;

    return QMessageBox::StandardButton::Yes;

}

void ScoreMenu::onQuitActionTriggered()
{
    AbcApplication* a = static_cast<AbcApplication*>(qApp);
    EditTabWidget* tabs = a->mainWindow()->mainHSplitter()->editTabWidget();
    if (QMessageBox::StandardButton::Yes == gracefulQuit()) {
            tabs->removeTabs();
            a->quit();
    }
}

void ScoreMenu::onOpenActionTriggered()
{
    AbcApplication* a = static_cast<AbcApplication*>(qApp);
    AbcMainWindow* w = a->mainWindow();

    QString  home = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
    QString fileName = QFileDialog::getOpenFileName(w, tr("Open ABC score"), home, tr("ABC score (*.abc)"));

    /* user cancelled */
    if (fileName.isEmpty())
        return;

    QFile file(fileName);
    if (file.open(QFile::ReadOnly | QFile::Text)) {
        EditTabWidget *edittabs = w->mainHSplitter()->editTabWidget();

        EditWidget* widget = new EditWidget(fileName, nullptr);

        AbcPlainTextEdit *edit = widget->editVBoxLayout()->abcPlainTextEdit();
        edit->setPlainText(file.readAll());
        file.close();
        edit->setSaved();
        edittabs->addTab(widget);
    }
}

void ScoreMenu::onSaveActionTriggered()
{
    AbcApplication* a = static_cast<AbcApplication*>(qApp);
    AbcMainWindow* w = a->mainWindow();
    EditTabWidget *edittabs = w->mainHSplitter()->editTabWidget();

    int cur = edittabs->currentIndex();
    if (cur < 0)
        return;

    QString fileName = (*edittabs->currentEditWidget()->fileName());
    if (fileName.isEmpty()) {
        QMessageBox::warning(this, tr("Warning"), tr("Could not save an untitled ABC file!"));
        return;
    }

    QFile file(fileName);
    if (file.open(QFile::WriteOnly | QFile::Text)) {
        AbcApplication* a = static_cast<AbcApplication*>(qApp);
        EditTabWidget *edittabs = a->mainWindow()->mainHSplitter()->editTabWidget();
        EditWidget* widget = static_cast<EditWidget*>(edittabs->currentWidget());
        AbcPlainTextEdit *edit = widget->editVBoxLayout()->abcPlainTextEdit();
        QString tosave = edit->toPlainText();
        file.write(tosave.toUtf8());
        file.close();
        edit->setSaved();
        w->statusBar()->showMessage(tr("Score saved."));
    } else {
        QMessageBox::warning(this, tr("Warning"), tr("Could not save ABC score!"));
    }
}

void ScoreMenu::onSaveAsActionTriggered()
{
    AbcApplication* a = static_cast<AbcApplication*>(qApp);
    AbcMainWindow* w = a->mainWindow();
    EditTabWidget *edittabs = w->mainHSplitter()->editTabWidget();
    int cur = edittabs->currentIndex();
    if (cur < 0)
        return;

    QString  home = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
    home += QDir::separator();
    home += "score.abc";
    QString fileName = QFileDialog::getSaveFileName(w, tr("Save ABC score"), home, tr("ABC score (*.abc)"));
    if (fileName.isEmpty())
        return; /* cancelled */

    QFileInfo info(fileName);
    edittabs->setTabText(edittabs->currentIndex(), info.baseName());
    edittabs->currentEditWidget()->setFileName(fileName);
    return onSaveActionTriggered();
}

void ScoreMenu::onExportActionTriggered()
{
    AbcApplication* a = static_cast<AbcApplication*>(qApp);
    AbcMainWindow* w = a->mainWindow();
    EditTabWidget *edittabs = w->mainHSplitter()->editTabWidget();
    int cur = edittabs->currentIndex();
    if (cur < 0)
        return;

    QString exp = *edittabs->currentEditWidget()->fileName();
    exp.replace(QRegularExpression("\\.abc$"), ".mid");
    QString fileName = QFileDialog::getSaveFileName(w, tr("Export MIDI file"), exp, tr("MIDI file (*.mid)"));
    if (fileName.isEmpty())
        return; /* cancelled */

    EditWidget* ew = edittabs->editWidgetList()->at(cur);
    ew->editVBoxLayout()->exportMIDI(fileName);
}

void ScoreMenu::onExportPsActionTriggered()
{
    AbcApplication* a = static_cast<AbcApplication*>(qApp);
    AbcMainWindow* w = a->mainWindow();
    EditTabWidget *edittabs = w->mainHSplitter()->editTabWidget();
    int cur = edittabs->currentIndex();
    if (cur < 0)
        return;

    QString exp = *edittabs->currentEditWidget()->fileName();
    exp.replace(QRegularExpression("\\.abc$"), ".ps");
    QString fileName = QFileDialog::getSaveFileName(w, tr("Export Postscript file"), exp, tr("Postscript file (*.ps)"));
    if (fileName.isEmpty())
        return; /* cancelled */

    EditWidget* ew = edittabs->editWidgetList()->at(cur);
    ew->editVBoxLayout()->exportPostscript(fileName);
}

void ScoreMenu::onCloseActionTriggered()
{
    AbcApplication* a = static_cast<AbcApplication*>(qApp);
    AbcMainWindow* w = a->mainWindow();
    EditTabWidget *edittabs = w->mainHSplitter()->editTabWidget();

    int cur = edittabs->currentIndex();
    if (cur < 0)
        return;

    edittabs->askRemoveTab(cur);
}

void ScoreMenu::onNewActionTriggered()
{
    AbcApplication* a = static_cast<AbcApplication*>(qApp);
    AbcMainWindow* w = a->mainWindow();
    EditTabWidget *edittabs = w->mainHSplitter()->editTabWidget();

    QString empty;
    EditWidget* swidget = new EditWidget(empty, nullptr);
    swidget->editVBoxLayout()->abcPlainTextEdit()->setPlainText(NEW_TEMPLATE);

    edittabs->addTab(swidget);
}
