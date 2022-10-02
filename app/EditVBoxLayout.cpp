#include "config.h"
#include "EditVBoxLayout.h"
#include "abcsynth.h"
#include "AbcApplication.h"
#include "PreferencesMenu.h"
#include "sfloader.h"
#include <QFileInfo>
#include <QDebug>
#include <QSpinBox>
#include <QDir>
#include <QStandardPaths>
#include <QMessageBox>
#include <QTimer>
#include "settings.h"
#include "../abc/abc.h"
#include "abcsmf.h"
#ifdef USE_LIBABCM2PS
#include "../abcm2ps/abcm2ps.h"
#endif

EditVBoxLayout::EditVBoxLayout(const QString& fileName, QWidget* parent)
	: QVBoxLayout(parent),
	abcplaintextedit(parent),
	playpushbutton(parent),
	runpushbutton(parent),
	hboxlayout(parent),
	xspinbox(parent),
	xlabel(parent),
    fileName(fileName),
    progress(nullptr),
    synth(nullptr),
    psgen(this),
    svggen(this),
    midigen(this)
{
    QString t = QDir::tempPath() + QDir::separator() + "redr-XXXXXX.abc";
	tempFile.setFileTemplate(t);
    xspinbox.setMinimum(1);
	xlabel.setText(tr("X:"));
	xlabel.setAlignment(Qt::AlignRight|Qt::AlignVCenter);
    xlabel.setBuddy(&xspinbox);
    progress = new QProgressIndicator();
    progress->setColor(qApp->palette().color(QPalette::Text));
    progress->startAnimation();

    hboxlayout.addWidget(&xlabel);
	hboxlayout.addWidget(&xspinbox);
    hboxlayout.addWidget(progress);
    hboxlayout.addWidget(&playpushbutton);
	hboxlayout.addWidget(&runpushbutton);

	addWidget(&abcplaintextedit);
	addLayout(&hboxlayout);

    connect(&abcplaintextedit, &QPlainTextEdit::selectionChanged, this, &EditVBoxLayout::onSelectionChanged);
    connect(&abcplaintextedit, &AbcPlainTextEdit::playableNote, this, &EditVBoxLayout::onPlayableNote);
    connect(&xspinbox, SIGNAL(valueChanged(int)), this, SLOT(onXChanged(int)));
	connect(&playpushbutton, SIGNAL(clicked()), this, SLOT(onPlayClicked()));
	connect(&runpushbutton, &QPushButton::clicked, this, &EditVBoxLayout::onRunClicked);

    connect(this, &EditVBoxLayout::doExportMIDI, this, &EditVBoxLayout::exportMIDI);

    connect(&psgen, &PsGenerator::generated, this, &EditVBoxLayout::onCompileFinished);
    connect(&svggen, &SvgGenerator::generated, this, &EditVBoxLayout::onCompileFinished);
    connect(&midigen, &MidiGenerator::generated, this, &EditVBoxLayout::onGenerateMIDIFinished);

    QFileInfo info(fileName);
    synth = new AbcSynth(info.baseName(), this);
    connect(synth, &AbcSynth::initFinished, this, &EditVBoxLayout::onSynthInited);
    connect(synth, &AbcSynth::synthFinished, this, &EditVBoxLayout::onSynthFinished);
    playpushbutton.setEnabled(false);
}

EditVBoxLayout::~EditVBoxLayout()
{
    /* stop synthesis */
    if (synth && synth->isPlaying()) {
        synth->stop();
        synth->waitFinish();
    }

    delete synth;

    removeMIDIFile();
    removeSvgFiles();
}

void EditVBoxLayout::finalize()
{
   cleanupThreads();
   cleanupProcesses();
}

void EditVBoxLayout::onSynthInited(bool err) {
    AbcApplication *a = static_cast<AbcApplication*>(qApp);
    progress->stopAnimation();
    progress->deleteLater();
    progress = nullptr;
    playpushbutton.setEnabled(true);
    if (err)
        a->mainWindow()->statusBar()->showMessage(tr("Cannot load sound font."));
}

void EditVBoxLayout::removeMIDIFile() {
    /* cleanup files manually */
    QString temp(tempFile.fileName());
    qDebug() << "cleaning MIDI for" << temp;
    temp.replace(QRegularExpression("\\.abc$"), QString::number(xspinbox.value())  + ".mid");
    if (QFileInfo::exists(temp))
        QFile::remove(temp);
}

void EditVBoxLayout::removeSvgFiles() {
    QString temp(tempFile.fileName());
    qDebug() << "cleaning SVG for" << temp;
    for (int i = 1; i < 999; i++) {
        temp = tempFile.fileName();
        temp.replace(QRegularExpression("\\.abc$"), QString::asprintf("%03d.svg", i));
        if (!QFileInfo::exists(temp))
            break;
        QFile::remove(temp);
    }
}

AbcPlainTextEdit *EditVBoxLayout::abcPlainTextEdit()
{
	return &abcplaintextedit;
}

PlayPushButton *EditVBoxLayout::playPushButton()
{
	return &playpushbutton;
}

RunPushButton *EditVBoxLayout::runPushButton()
{
	return &runpushbutton;
}

void EditVBoxLayout::setFileName(const QString &fn)
{
    fileName = fn;
}

void EditVBoxLayout::cleanupProcesses()
{
    /* early kill in case we quit the app brutally */
    for (int i = 0; i < processlist.length(); i++)
        processlist.at(i)->kill();
}

void EditVBoxLayout::cleanupThreads()
{
    if (synth->isLoading())
        synth->abort();
    else if (synth->isPlaying())
        synth->stop();
}

void EditVBoxLayout::onXChanged(int value)
{
    qDebug() << value;
    onRunClicked();
}

void EditVBoxLayout::onPlayClicked()
{
    static QString dot;
    AbcApplication *a = static_cast<AbcApplication*>(qApp);
    if (a->isQuit())
        return;

    if (synth->isLoading()){
        a->mainWindow()->statusBar()->showMessage(tr("Please wait...") + dot);
        dot += " ...";
        return;
    }

    if (playpushbutton.isPlay()) {
        a->mainWindow()->statusBar()->showMessage(tr("Generating MIDI for playing."));
        playpushbutton.flip();
        xspinbox.setEnabled(false);
        playpushbutton.setEnabled(false);
        QApplication::processEvents();
        emit doExportMIDI(QString());
      } else { /* stop */
        if (synth->isPlaying()) {
            dot.clear();
            a->mainWindow()->statusBar()->showMessage(tr("Stopping synthesis..."));
            synth->stop();
        }
    }
}

QString EditVBoxLayout::constructHeaders() {
    QString headers;

    QString all = abcPlainTextEdit()->toPlainText();
    int i = 0, xl = 0;
    QStringList lines = all.split('\n');

    /* find last X: before selectionIndex */
    for (int l = 0; l < lines.count() && i < selectionIndex; l++) {
        i += lines.at(l).count() +1; /* count \n */
        if (lines.at(l).startsWith("X:")) {
            xspinbox.setValue(lines.at(l).right(1).toInt());
            xl = l;
            /* don't break on first X: continue until selectionIndex */
        }
    }

    /* construct headers */
    for (int j = xl;  j < lines.count(); j++) {
        /* stop at 'V:' but include all other headers and comments above it */
        if (lines.at(j).contains(QRegularExpression("^((%[^\n]*)|([A-UW-Z]:[^\n]+))$"))) {
            headers += lines.at(j) + "\n";
        } else /* found 'V:' or notes */
            break;
    }

    return headers;
}

void EditVBoxLayout::exportMIDI(QString filename) {
    QString tosave;

    if (selection.isEmpty()) {
        tosave = abcPlainTextEdit()->toPlainText();
    } else {
        tosave = constructHeaders();
        /* when coming from QTextCursor::selectedText(), LF is replaced by U+2029! */
        tosave += selection.replace(QChar::ParagraphSeparator, "\n");
    }

    /* open tempfile to init a name */
    tempFile.open();
    tempFile.write(tosave.toUtf8());
    tempFile.close();

    int cont;
    if (filename.isEmpty()) {
        cont = 1; /* continue to playback */
    } else {
        cont = 0; /* will not play, it's just an export */
    }

    Settings settings;
    QVariant player = settings.value(PLAYER_KEY);

    if (player == LIBABC2SMF) {
        QByteArray ba = tosave.toUtf8();

        if (filename.isEmpty()) {
            filename = tempFile.fileName();
            filename.replace(QRegularExpression("\\.abc$"), QString::number(xspinbox.value()) + ".mid");
        }

        midigen.generate(ba, tempFile.fileName(), xspinbox.value(), filename, cont);
    } else {
        midigen.generate(tempFile.fileName(), xspinbox.value(), filename, cont);
    }
}

void EditVBoxLayout::exportPostscript(QString filename)
{
    AbcApplication *a = static_cast<AbcApplication*>(qApp);
    if (a->isQuit())
        return;

    a->mainWindow()->statusBar()->showMessage(tr("Exporting score..."));
    QString tosave = abcPlainTextEdit()->toPlainText();
    tempFile.open();
    tempFile.write(tosave.toUtf8());
    tempFile.close();
    psgen.generate(tempFile.fileName(), xspinbox.value(), filename, 0);
}

void EditVBoxLayout::onGenerateMIDIFinished(int exitCode, const QString& errstr, int cont)
{
    playpushbutton.setEnabled(true);
    AbcApplication *a = static_cast<AbcApplication*>(qApp);
    if (a->isQuit())
        return;

    if (exitCode) {
        a->mainWindow()->statusBar()->showMessage(tr("Error during MIDI generation."));

        if (selection.isEmpty())
            QMessageBox::warning(a->mainWindow(), tr("Error"), errstr);
        else
            QMessageBox::warning(a->mainWindow(), tr("Error"), tr("Parse error in selected notes."));

        if (cont) {
            playpushbutton.flip();
            xspinbox.setEnabled(true);
        }
    } else {
        a->mainWindow()->statusBar()->showMessage(tr("MIDI generation finished."));
        if (cont) {
            if (synth->isPlaying())
                synth->waitFinish();

            /* midi file can change from tune (xspinbox) index */
            QString midifile(tempFile.fileName());
            midifile.replace(QRegularExpression("\\.abc$"), QString::number(xspinbox.value())  + ".mid");
            synth->play(midifile);
        }
    }
}

void EditVBoxLayout::onSynthFinished(bool err)
{
    AbcApplication *a = static_cast<AbcApplication*>(qApp);
    if (err) {
        a->mainWindow()->statusBar()->showMessage(tr("Error during synthesis."));
    } else {
        a->mainWindow()->statusBar()->showMessage(tr("Synthesis finished."));
    }

    int x = xspinbox.value();
    QString midi (tempFile.fileName());
    midi.replace(QRegularExpression("\\.abc$"), QString::number(x) + ".mid");
    QFile::remove(midi);

    playpushbutton.flip();
    xspinbox.setEnabled(true);
}

void EditVBoxLayout::onRunClicked()
{
    runpushbutton.setEnabled(false);
    /* do not disable/enable xspinbox because Play manages it for audio rendering! */

    AbcApplication *a = static_cast<AbcApplication*>(qApp);
    if (a->isQuit())
        return;

    /* as we do not know the number of previous pages (it could be more or less)
     * we have to remove bruteforcely them */
    removeSvgFiles();

    //if (a->mainWindow()->statusBar()->currentMessage().isEmpty())
    //    a->mainWindow()->statusBar()->showMessage(tr("Generating score..."));
    QString tosave = abcPlainTextEdit()->toPlainText();
    tempFile.open();
    tempFile.write(tosave.toUtf8());
    tempFile.close();
    svggen.generate(tempFile.fileName(), xspinbox.value(), QString(), 1);
}

int EditVBoxLayout::xOfCursor(const QTextCursor& c) {
    int index = c.selectionStart();
    QString all = abcPlainTextEdit()->toPlainText();
    int x = 1;
    int i = 0;
    QStringList lines = all.split('\n');

    /* find last X: before selectionIndex */
    for (int l = 0; l < lines.count() && i < index; l++) {
        i += lines.at(l).count() +1; /* count \n */
        if (lines.at(l).startsWith("X:")) {
            x = lines.at(l).right(1).toInt();
        }
    }

    return x;
}

void EditVBoxLayout::onSelectionChanged()
{
    QTextCursor c = abcPlainTextEdit()->textCursor();
    if (c.hasSelection()) {
        selection = c.selectedText();
        selectionIndex = c.selectionStart();
    } else {
        selection.clear();
        selectionIndex = c.selectionStart();
        /* set X spinbox */
        int x = xOfCursor(c);
        xspinbox.setValue(x);
    }
}

void EditVBoxLayout::onPlayableNote(const QString &note)
{
    QString abc = constructHeaders();
    abc.append(note);
#if 0
    const QDataStream* stream = midigen.generate(abc.toUtf8(), xOfCursor(abcplaintextedit.textCursor()));
    QIODevice* dev = stream->device();
    dev->seek(0);
    QByteArray ba = stream->device()->readAll();
    synth->play(ba);
    delete stream;
#else
    int c = -1, p = 0, k = -1;
    if (midigen.genFirstNote(abc, &c, &p, &k)) {
        synth->fire(c, p, k, 80);
    }
#endif
}

void EditVBoxLayout::onCompileFinished(int exitCode, const QString& errstr, int cont)
{
    AbcApplication *a = static_cast<AbcApplication*>(qApp);
    if (a->isQuit())
        return;

    qDebug() << "compile" << exitCode;

    if (exitCode < 0 || exitCode > 1) { /* sometimes, abcm2ps returns 1 even on 'success' */
        if (cont) {
            a->mainWindow()->statusBar()->showMessage(tr("Error during score generation."));
        } else {
            QMessageBox::warning(a->mainWindow(), tr("Error"), errstr);
        }

        return;
    }

    if (a->mainWindow()->statusBar()->currentMessage().isEmpty())
        a->mainWindow()->statusBar()->showMessage(tr("Score generated."));

    if (!cont) {
        runpushbutton.setEnabled(true);
        return;
    }

    QFileInfo info(tempFile);
    QString b(info.baseName());
    QString d = info.dir().absolutePath();
    a->mainWindow()->mainHSplitter()->viewWidget()->cleanup();
    a->mainWindow()->mainHSplitter()->viewWidget()->initBasename(b, d);
    a->mainWindow()->mainHSplitter()->viewWidget()->requestPage(1);

    runpushbutton.setEnabled(true);
}
