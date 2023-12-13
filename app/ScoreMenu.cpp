#include "ScoreMenu.h"
#include "AbcApplication.h"
#include "AbcPlainTextEdit.h"
#include "EditTabWidget.h"
#include "EditWidget.h"
#include "settings.h"
#include "wizard.h"
#include <QMessageBox>
#include <QApplication>
#include <QFileDialog>
#include <QStandardPaths>
#include <QDebug>

const QRegularExpression ScoreMenu::m_abcext("\\.abc$");

ScoreMenu::ScoreMenu(QWidget* parent)
	: QMenu(parent)
{
    setTitle(tr("Score"));

    addAction(tr("New"), this, &ScoreMenu::onNewActionTriggered, QKeySequence::New);
    addAction(tr("Open"), this, &ScoreMenu::onOpenActionTriggered, QKeySequence::Open);

    for (int i = 0; i < MaxRecentFiles; ++i) {
        recentFileActs[i] = new QAction(this);
        recentFileActs[i]->setVisible(false);
        connect(recentFileActs[i], &QAction::triggered, this, &ScoreMenu::onOpenRecentActionTriggered);
    }

    QMenu* sub = addMenu(tr("Recently opened"));
    for (int i = 0; i < MaxRecentFiles; ++i)
        sub->addAction(recentFileActs[i]);

    updateRecentFileActions();

    addAction(tr("Save"), this, &ScoreMenu::onSaveActionTriggered, QKeySequence::Save);
    addAction(tr("Save as"), this, &ScoreMenu::onSaveAsActionTriggered, QKeySequence::SaveAs);
    addAction(tr("Export to MIDI"), this, &ScoreMenu::onExportActionTriggered);
    addAction(tr("Export to Postscript"), this, &ScoreMenu::onExportPsActionTriggered);
    //addAction(tr("Export to PDF"), this, &ScoreMenu::onExportPdfActionTriggered); FIXME libspectre export fails.
    addAction(tr("Close"), this, &ScoreMenu::onCloseActionTriggered, QKeySequence::Close);
    addAction(tr("Quit"), this, &ScoreMenu::onQuitActionTriggered, QKeySequence::Quit);
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

    //QString  home = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
    QString fileName = QFileDialog::getOpenFileName(w, tr("Open ABC score"), QString(), tr("ABC score (*.abc)"));

    /* user cancelled */
    if (fileName.isEmpty())
        return;

    loadFile(fileName);
}

QString ScoreMenu::strippedName(const QString& fullFileName)
{
    return QFileInfo(fullFileName).fileName();
}

void ScoreMenu::generateTemplate(QString &abc, Wizard::Template tmpl)
{
    int voices = 1;
    QString grouping;

    switch (tmpl) {
    case Wizard::TemplateKeyboard:
        voices = 2;
        grouping = "{";
        break;
    case Wizard::TemplateString4tet:
        voices = 4;
        grouping = "[";
        break;
    case Wizard::TemplateSATBChoir:
        voices = 4;
        break;
    case Wizard::TemplatePercussion:
        voices = 3;
        grouping = "(";
    case Wizard::TemplateNone:
    default:
        break;
    }

    if (!grouping.isEmpty()) {
        abc = abc.append("%%staves ");
        abc = abc.append(grouping);
        for (int i = 0; i < voices; i++) {
            abc = abc.append(" %1").arg(i+1);
        }

        if (grouping == "{")
            abc = abc.append(" }\n");
        else if (grouping == "[")
            abc = abc.append(" ]\n");
        else /* default to ) */
            abc = abc.append(" )\n");
    }

    for (int i = 1; i <= voices; i++) {
        abc = abc.append("V:%1").arg(i);
        if (tmpl == Wizard::TemplateKeyboard) {
            switch (i) {
            case 1:
                abc = abc.append(" clef=treble\n");
                abc = abc.append("%%MIDI program 0 % Acoustic Grand Piano\n");
                abc = abc.append("C8|]\n");
                break;
            case 2:
                abc = abc.append(" clef=bass\n");
                abc = abc.append("%%MIDI program 0 % Acoustic Grand Piano\n");
                abc = abc.append("C,8|]\n");
                break;
            }
        } else if (tmpl == Wizard::TemplateString4tet) {
            switch (i) {
            case 1:
                abc = abc.append(" clef=treble name=Violin1 sname=Vln.1\n");
                abc = abc.append("%%MIDI program 40 % Violin\n");
                abc = abc.append("c8|]\n");
                break;
            case 2:
                abc = abc.append(" clef=treble name=Violin2 sname=Vln.2\n");
                abc = abc.append("%%MIDI program 40 % Violin\n");
                abc = abc.append("G8|]\n");
                break;
            case 3:
                abc = abc.append(" clef=alto name=Viola sname=Vla.\n");
                abc = abc.append("%%MIDI program 41 % Viola\n");
                abc = abc.append("E8|]\n");
                break;
            case 4:
                abc = abc.append(" clef=bass name=Cello sname=Clo.\n");
                abc = abc.append("%%MIDI program 42 % Cello\n");
                abc = abc.append("C8|]\n");
                break;
            }
        } else if (tmpl == Wizard::TemplateSATBChoir) {
            switch (i) {
            case 1:
                abc = abc.append(" clef=treble name=Soprano sname=S.\n");
                abc = abc.append("%%MIDI program 53 % Voice Oohs\n");
                abc = abc.append("c8|]\n");
                break;
            case 2:
                abc = abc.append(" clef=treble name=Alto sname=A.\n");
                abc = abc.append("%%MIDI program 53 % Voice Oohs\n");
                abc = abc.append("E8|]\n");
                break;
            case 3:
                abc = abc.append(" clef=tenor name=Tenor sname=T.\n");
                abc = abc.append("%%MIDI program 53 % Voice Oohs\n");
                abc = abc.append("G,8|]\n");
                break;
            case 4:
                abc = abc.append(" clef=bass name=Bass sname=B.\n");
                abc = abc.append("%%MIDI program 53 % Voice Oohs\n");
                abc = abc.append("C,8|]\n");
                break;
            }
        } else if (tmpl == Wizard::TemplatePercussion) {
            switch (i) {
            case 1:
                abc = abc.append(" clef=perc\n");
                abc = abc.append("%%MIDI program 119 % Reverse Cymbal\n");
                abc = abc.append(("z6 ^c'2|]\n"));
                break;
             case 2:
                abc = abc.append(" clef=perc\n");
                abc = abc.append("%%MIDI program 117 % Melodic Drum\n");
                abc = abc.append("%%MIDI transpose -12\n");
                abc = abc.append("^c/2^c/2^c/2^c/2^G/2^G/2^G/2^G/2^C/2^C/2^C/2^C/2 z2|]\n");
                break;
            case 3:
                abc = abc.append(" clef=perc\n");
                abc = abc.append("%%MIDI program 117 % Melodic Drum\n");
                abc = abc.append("%%MIDI transpose -24\n");
                abc = abc.append("z6 G,2|]\n");
                break;
           }
        }
    }
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
#if 0
    if (!ew->editVBoxLayout()->abcPlainTextEdit()->isSaved()) {
        QMessageBox::warning(w, tr("Warning"), tr("Please save score before to export."));
        return;
    }
#endif
    QString exp = *ew->fileName();
    exp.replace(m_abcext, ".mid");
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
#if 0
    if (!ew->editVBoxLayout()->abcPlainTextEdit()->isSaved()) {
        QMessageBox::warning(w, tr("Warning"), tr("Please save score before to export."));
        return;
    }
#endif
    QString exp = *ew->fileName();
    exp.replace(m_abcext, ".ps");
    QString fileName = QFileDialog::getSaveFileName(w, tr("Export Postscript file"), exp, tr("Postscript file (*.ps)"));
    if (fileName.isEmpty())
        return; /* cancelled */

    ew->editVBoxLayout()->exportPS(fileName);
}

void ScoreMenu::onExportPdfActionTriggered()
{
    AbcApplication* a = static_cast<AbcApplication*>(qApp);
    AbcMainWindow* w = a->mainWindow();
    EditTabWidget *edittabs = w->mainHSplitter()->editTabWidget();
    int cur = edittabs->currentIndex();
    if (cur < 0)
        return;

    EditWidget* ew = qobject_cast<EditWidget*>(edittabs->currentWidget());
#if 0
    if (!ew->editVBoxLayout()->abcPlainTextEdit()->isSaved()) {
        QMessageBox::warning(w, tr("Warning"), tr("Please save score before to export."));
        return;
    }
#endif
    QString exp = *ew->fileName();
    exp.replace(m_abcext, ".pdf");
    QString fileName = QFileDialog::getSaveFileName(w, tr("Export PDF file"), exp, tr("PDF file (*.pdf)"));
    if (fileName.isEmpty())
        return; /* cancelled */

    ew->editVBoxLayout()->exportPDF(fileName);
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
#if 0
    AbcApplication* a = static_cast<AbcApplication*>(qApp);
    AbcMainWindow* w = a->mainWindow();
    EditTabWidget *edittabs = w->mainHSplitter()->editTabWidget();

    QString empty;
    EditWidget* swidget = new EditWidget(empty, nullptr);
    swidget->editVBoxLayout()->abcPlainTextEdit()->setPlainText(NEW_TEMPLATE);

    edittabs->addTab(swidget);
#else
    AbcApplication* a = static_cast<AbcApplication*>(qApp);
    Wizard* wiz = new Wizard(a->mainWindow());
    wiz->exec();

    QString abc("%abc\nX:1\nT:%1\nC:%2\nQ:1/4=120\nL:1/8\nM:4/4\nK:CMaj\n");
    abc = abc.arg(wiz->title(tr("Title")), wiz->composer(tr("Composer")));

    if (wiz->templat() == Wizard::TemplateNone) {
        if (!wiz->grouping().isEmpty()) {
            abc = abc.append("%%staves %1").arg(wiz->grouping());
            for (int i = 1; i <= wiz->voices(); i++) {
                abc = abc.append(" ");
                abc = abc.append(QString::number(i));
            }

            if (wiz->grouping() == "{")
                abc.append(" }");
            else if (wiz->grouping() == "[")
                abc.append(" ]");

            abc = abc.append("\n");
        }

        for (int i = 1; i <= wiz->voices(); i++) {
            abc = abc.append("V:%1\n").arg(i);
            abc = abc.append("C8|]\n");
        }
    } else {
        generateTemplate(abc, wiz->templat());
    }

    EditTabWidget *edittabs = a->mainWindow()->mainHSplitter()->editTabWidget();
    EditWidget* widget = new EditWidget(QString(), nullptr);
    widget->editVBoxLayout()->abcPlainTextEdit()->setPlainText(abc);
    edittabs->addTab(widget);

    delete wiz;
#endif
}
