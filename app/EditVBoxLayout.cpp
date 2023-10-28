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

const QRegularExpression EditVBoxLayout::m_abcext(QStringLiteral("\\.abc$"));

EditVBoxLayout::EditVBoxLayout(const QString& fileName, QWidget* parent)
    : QVBoxLayout(parent),
      fileName(fileName),
      progress(nullptr),
      synth(nullptr),
      psgen(nullptr),
      midigen(nullptr)
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
    connect(&xspinbox, QOverload<int>::of(&QSpinBox::valueChanged), this, &EditVBoxLayout::onXChanged);
    connect(&playpushbutton, &QPushButton::clicked, this, &EditVBoxLayout::onPlayClicked);
    connect(&runpushbutton, &QPushButton::clicked, this, &EditVBoxLayout::onDisplayClicked);

    connect(this, &EditVBoxLayout::doExportMIDI, this, &EditVBoxLayout::exportMIDI);

    QFileInfo info(fileName);
    synth = new AbcSynth(info.baseName(), this);
    connect(synth, &AbcSynth::initFinished, this, &EditVBoxLayout::onSynthInited);
    connect(synth, &AbcSynth::synthFinished, this, &EditVBoxLayout::onSynthFinished);
    playpushbutton.setEnabled(false);
}

EditVBoxLayout::~EditVBoxLayout()
{
    finalize();

    delete synth;

    removeMIDIFile();
    removePSFile();
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
    temp.replace(m_abcext, QString::number(xspinbox.value())  + ".mid");
    if (QFileInfo::exists(temp))
        QFile::remove(temp);
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
        synth->abortSFLoad();
    else if (synth->isPlaying())
        synth->abortPlay();
}

void EditVBoxLayout::onXChanged(int value)
{
    qDebug() << value;
    onDisplayClicked();
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
            xspinbox.setValue(lines.at(l).rightRef(1).toInt());
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

    AbcProcess::Continuation cont;
    if (filename.isEmpty()) {
        cont = AbcProcess::ContinuationRender; /* continue to playback */
    } else {
        cont = AbcProcess::ContinuationNone; /* will not play, it's just an export */
    }

    Settings settings;
    QVariant player = settings.value(PLAYER_KEY);

    if (filename.isEmpty()) {
        filename = tempFile.fileName();
        filename.replace(m_abcext, QString::number(xspinbox.value()) + ".mid");
    }

    midigen = new MidiGenerator(filename, this);
    connect(midigen, &MidiGenerator::generated, this, &EditVBoxLayout::onGenerateMIDIFinished);

    if (player == LIBABC2SMF) {
        QByteArray ba = tosave.toUtf8();
        midigen->generate(ba, tempFile.fileName(), xspinbox.value(), cont);
    } else {
        midigen->generate(tempFile.fileName(), xspinbox.value(), cont);
    }
}

void EditVBoxLayout::removePSFile()
{
    /* cleanup files manually */
    QString temp(tempFile.fileName());
    qDebug() << "cleaning Ps for" << temp;
    temp.replace(m_abcext, ".ps");
    if (QFileInfo::exists(temp))
        QFile::remove(temp);
}

void EditVBoxLayout::exportPS(QString filename)
{
    AbcApplication *a = static_cast<AbcApplication*>(qApp);
    if (a->isQuit())
        return;

    a->mainWindow()->statusBar()->showMessage(tr("Exporting score..."));
    QString tosave = abcPlainTextEdit()->toPlainText();
    tempFile.open();
    tempFile.write(tosave.toUtf8());
    tempFile.close();
    psgen = new PsGenerator(filename, this);
    connect(psgen, &PsGenerator::generated, this, &EditVBoxLayout::onGeneratePSFinished);
    psgen->generate(tempFile.fileName(), xspinbox.value(), AbcProcess::Continuation::ContinuationNone);
}

void EditVBoxLayout::exportPDF(QString filename)
{
    AbcApplication *a = static_cast<AbcApplication*>(qApp);
    if (a->isQuit())
        return;

    a->mainWindow()->statusBar()->showMessage(tr("Exporting score..."));
    QString tosave = abcPlainTextEdit()->toPlainText();
    tempFile.open();
    tempFile.write(tosave.toUtf8());
    tempFile.close();
    pdfFileName = filename;
    /* we will generate a _temporary_ PS file */
    psgen = new PsGenerator(QString(), this);
    connect(psgen, &PsGenerator::generated, this, &EditVBoxLayout::onGeneratePSFinished);
    psgen->generate(tempFile.fileName(), xspinbox.value(), AbcProcess::Continuation::ContinuationConvert);
}

void EditVBoxLayout::onGenerateMIDIFinished(int exitCode, const QString& errstr, AbcProcess::Continuation cont)
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

        if (cont == AbcProcess::ContinuationRender) {
            playpushbutton.flip();
            xspinbox.setEnabled(true);
        }
    } else {
        a->mainWindow()->statusBar()->showMessage(tr("MIDI generation finished."));
        if (cont == AbcProcess::ContinuationRender) {
            if (synth->isPlaying())
                synth->stop();

            /* midi file can change from tune (xspinbox) index */
            QString midifile(tempFile.fileName());
            midifile.replace(m_abcext, QString::number(xspinbox.value())  + ".mid");
            synth->play(midifile);
        }
    }

    delete midigen;
}

void EditVBoxLayout::onSynthFinished(bool err)
{
    AbcApplication *a = static_cast<AbcApplication*>(qApp);
    if (err)
        a->mainWindow()->statusBar()->showMessage(tr("Synthesis error."));
    else
        a->mainWindow()->statusBar()->showMessage(tr("Synthesis finished."));

    int x = xspinbox.value();
    QString midi (tempFile.fileName());
    midi.replace(m_abcext, QString::number(x) + ".mid");
    QFile::remove(midi);

    playpushbutton.flip();
    xspinbox.setEnabled(true);
}

void EditVBoxLayout::popupWarning(const QString& title, const QString& text) {
    AbcApplication *a = static_cast<AbcApplication*>(qApp);
    QMessageBox::warning(a->mainWindow(), title, text);
}

void EditVBoxLayout::saveToPDF(const QString &outfile)
{
    QFileInfo info(psgen->outFile());
    if (!info.exists()) {
        popupWarning(tr("Error"), tr("Could not find PS score"));
        return;
    }

    spectre = spectre_document_new();

    spectre_document_load(spectre, psgen->outFile().toLocal8Bit().constData());

    if (spectre_document_status(spectre) != SPECTRE_STATUS_SUCCESS) {
        popupWarning(tr("Error"), tr("Could not load PS score"));
        //qWarning() << spectre_status_to_string(spectre_document_status(spectre));
        spectre_document_free(spectre);
        return;
    }

    spectre_document_save_to_pdf(spectre, outfile.toLocal8Bit().constData());

    if (spectre_document_status(spectre) != SPECTRE_STATUS_SUCCESS) {
        popupWarning(tr("Error"), tr("Could not save to PDF"));
        //qWarning() << spectre_status_to_string(spectre_document_status(spectre));
        spectre_document_free(spectre);
        return;
    }

    spectre_document_free(spectre);
}

void EditVBoxLayout::onDisplayClicked()
{
    runpushbutton.setEnabled(false);

    /* do not disable/enable xspinbox because Play manages it for audio rendering! */

    AbcApplication *a = static_cast<AbcApplication*>(qApp);
    if (a->isQuit())
        return;

    removePSFile();

    //if (a->mainWindow()->statusBar()->currentMessage().isEmpty())
    //    a->mainWindow()->statusBar()->showMessage(tr("Generating score..."));
    QString tosave = abcPlainTextEdit()->toPlainText();
    tempFile.open();
    tempFile.write(tosave.toUtf8());
    tempFile.close();
    psgen = new PsGenerator(QString(), this);
    connect(psgen, &PsGenerator::generated, this, &EditVBoxLayout::onGeneratePSFinished);
    psgen->generate(tempFile.fileName(), xspinbox.value(), AbcProcess::ContinuationRender);
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
            x = lines.at(l).rightRef(1).toInt();
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
    if (autoplayer.genFirstNote(abc, &c, &p, &k)) {
        synth->fire(c, p, k, 80);
    }
#endif
}

void EditVBoxLayout::onGeneratePSFinished(int exitCode, const QString &errstr, AbcProcess::Continuation cont)
{
    AbcApplication *a = static_cast<AbcApplication*>(qApp);
    if (a->isQuit())
        return;

    qDebug() << "ps" << exitCode;

    if (exitCode) {
        if (cont == AbcProcess::ContinuationRender) {
            a->mainWindow()->statusBar()->showMessage(tr("Error during score generation."));
        } else {
            QMessageBox::warning(a->mainWindow(), tr("Error"), errstr);
        }
    } else {
        a->mainWindow()->statusBar()->showMessage(tr("Score generated."));
    }

    if (cont == AbcProcess::ContinuationNone) {
        runpushbutton.setEnabled(true);
        return;
    } else if (cont == AbcProcess::ContinuationRender) {
        /* continuation: display PS */
        QFileInfo info(tempFile);
        QString b(info.baseName());
        QString d = info.dir().absolutePath();
        a->mainWindow()->mainHSplitter()->viewWidget()->cleanup();
        a->mainWindow()->mainHSplitter()->viewWidget()->initBasename(fileName, b, d);
        a->mainWindow()->mainHSplitter()->viewWidget()->requestPage(1);

    } else if (cont == AbcProcess::ContinuationConvert) /* PDF Export */ {
        /* continuation: convert to PDF */
        saveToPDF(pdfFileName);
    }

    delete psgen;
    runpushbutton.setEnabled(true);
}
