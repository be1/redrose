#include "ScoreMenu.h"
#include "AbcApplication.h"
#include "AbcPlainTextEdit.h"
#include "EditTabWidget.h"
#include "EditWidget.h"
#include "settings.h"
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

    for (int i = 0; i < MaxRecentFiles; ++i) {
        recentFileActs[i] = new QAction(this);
        recentFileActs[i]->setVisible(false);
        connect(recentFileActs[i], &QAction::triggered, this, &ScoreMenu::onOpenRecentActionTriggered);
    }

    QMenu* sub = addMenu(tr("Recently opened"));
    for (int i = 0; i < MaxRecentFiles; ++i)
        sub->addAction(recentFileActs[i]);

    updateRecentFileActions();

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
    for (int i = 0; i < tabs->count(); i++) {
        EditWidget* w = qobject_cast<EditWidget*>(tabs->widget(i));
        if (!w->editVBoxLayout()->abcPlainTextEdit()->isSaved())
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

    loadFile(fileName);
}

QString ScoreMenu::strippedName(const QString& fullFileName)
{
    return QFileInfo(fullFileName).fileName();
}

void ScoreMenu::updateRecentFileActions()
{
    Settings settings;
    QStringList files = settings.value("recentFileList").toStringList();

    int numRecentFiles = qMin(files.size(), (int)MaxRecentFiles);

    for (int i = 0; i < numRecentFiles; ++i) {
        QString text = tr("&%1 %2").arg(i + 1).arg(strippedName(files[i]));
        recentFileActs[i]->setText(text);
        recentFileActs[i]->setData(files[i]);
        recentFileActs[i]->setVisible(true);
    }

    for (int j = numRecentFiles; j < MaxRecentFiles; ++j)
        recentFileActs[j]->setVisible(false);
}

void ScoreMenu::setRecentFile(const QString& fileName, bool ok)
{
    Settings settings;
    QStringList files = settings.value("recentFileList").toStringList();
    files.removeAll(fileName);

    if (ok)
        files.prepend(fileName);

    while (files.size() > MaxRecentFiles)
        files.removeLast();

    settings.setValue("recentFileList", files);
    updateRecentFileActions();
}

bool ScoreMenu::loadFile(const QString& fileName)
{
    AbcApplication* a = static_cast<AbcApplication*>(qApp);
    AbcMainWindow* w = a->mainWindow();

    QFile file(fileName);
    if (file.open(QFile::ReadOnly | QFile::Text)) {
        EditTabWidget *edittabs = w->mainHSplitter()->editTabWidget();

        EditWidget* widget = new EditWidget(fileName, edittabs);

        AbcPlainTextEdit *edit = widget->editVBoxLayout()->abcPlainTextEdit();
        edit->setPlainText(file.readAll());
        file.close();
        edit->setSaved();
        edittabs->addTab(widget);

        setRecentFile(fileName, true);

        return true;
    }

    setRecentFile(fileName, false);
    return false;
}

void ScoreMenu::onOpenRecentActionTriggered()
{
    AbcApplication* a = static_cast<AbcApplication*>(qApp);
    AbcMainWindow* w = a->mainWindow();

    QAction *action = qobject_cast<QAction *>(sender());
    if (action) {
        if (!loadFile(action->data().toString())) {
            QMessageBox::warning(w, tr("Warning"), tr("Could not open score!"));
        }
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

    QString fileName = *qobject_cast<EditWidget*>(edittabs->currentWidget())->fileName();
    if (fileName.isEmpty()) {
        QMessageBox::warning(w, tr("Warning"), tr("Could not save an untitled ABC file!"));
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
        setRecentFile(fileName, true);
        w->statusBar()->showMessage(tr("Score saved."));
    } else {
        QMessageBox::warning(w, tr("Warning"), tr("Could not save ABC score!"));
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

    if (!fileName.endsWith(".abc"))
        fileName.append(".abc");

    QFileInfo info(fileName);
    edittabs->setTabText(edittabs->currentIndex(), info.baseName());
    EditWidget* ew = qobject_cast<EditWidget*>(edittabs->currentWidget());
    ew->setFileName(fileName);
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

    EditWidget* ew = qobject_cast<EditWidget*>(edittabs->currentWidget());
    QString exp = *ew->fileName();
    exp.replace(QRegularExpression("\\.abc$"), ".mid");
    QString fileName = QFileDialog::getSaveFileName(w, tr("Export MIDI file"), exp, tr("MIDI file (*.mid)"));
    if (fileName.isEmpty())
        return; /* cancelled */

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

    EditWidget* ew = qobject_cast<EditWidget*>(edittabs->currentWidget());
    QString exp = *ew->fileName();
    exp.replace(QRegularExpression("\\.abc$"), ".ps");
    QString fileName = QFileDialog::getSaveFileName(w, tr("Export Postscript file"), exp, tr("Postscript file (*.ps)"));
    if (fileName.isEmpty())
        return; /* cancelled */

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
