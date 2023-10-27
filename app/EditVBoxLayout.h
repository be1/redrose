#ifndef EDITVBOXLAYOUT_H
#define EDITVBOXLAYOUT_H

#include "RunPushButton.h"
#include "PlayPushButton.h"
#include "AbcPlainTextEdit.h"
#include "AbcProcess.h"
#include "AbcTemporaryFile.h"
#include "abcsynth.h"
#include "psgenerator.h"
#include "midigenerator.h"
#include "QProgressIndicator.h"
#include <QVBoxLayout>
#include <QSpinBox>
#include <QLabel>
#include <QDir>
#include <QTemporaryFile>
#include <QThread>
#include <libspectre/spectre-document.h>

class EditVBoxLayout: public QVBoxLayout
{
	Q_OBJECT

public:
    explicit EditVBoxLayout(const QString& fileName, QWidget* parent = nullptr);
	~EditVBoxLayout();

    void finalize(void);
    AbcPlainTextEdit *abcPlainTextEdit();
    PlayPushButton *playPushButton();
    RunPushButton *runPushButton();
    void setFileName(const QString& fn);

    /**
     * @brief exportPostscript
     * @param filename Output file path. Can be empty (has defaults).
     */
    void exportPS(QString filename);
    void exportPDF(QString filename);
    /**
     * @brief exportMIDI
     * @param filename Output file path. Can be emtpy (has defaults).
     */
    void exportMIDI(QString filename);

signals:
    void doExportMIDI(const QString& outfilename);

protected:
    void spawnProgram(const QString& prog, const QStringList &args, AbcProcess::ProcessType which, const QDir &wrk, enum AbcProcess::Continuation cont);
    void removePSFile();
    void removeMIDIFile();
    int xOfCursor(const QTextCursor& c);
    void cleanupProcesses();
    void cleanupThreads();
    QString constructHeaders();

public slots:
    void onXChanged(int value);
    void onPlayClicked(); /* midi */
    void onDisplayClicked(); /* ps */
    void onSelectionChanged();

protected slots:
    void onPlayableNote(const QString& note);
    void onGeneratePSFinished(int exitCode, const QString& errstr, enum AbcProcess::Continuation cont);
    void onGenerateMIDIFinished(int exitCode, const QString& errstr, enum AbcProcess::Continuation cont);
    void onSynthInited(bool err);
    void onSynthFinished(bool err);
    void saveToPDF(const QString& outfile);
    void popupWarning(const QString& title, const QString& text);

private:
	AbcPlainTextEdit abcplaintextedit;
    PlayPushButton playpushbutton; /* midi */
    RunPushButton runpushbutton; /* svg */
    QHBoxLayout hboxlayout;
    QSpinBox xspinbox;
    QLabel xlabel;
    QString fileName; /* original abc file name */
    QString pdfFileName; /* needed for PDF export */
    AbcTemporaryFile tempFile;
    QList<AbcProcess*> processlist;
    QProgressIndicator* progress;
    QString selection;
    int selectionIndex;

    AbcSynth* synth;
    PsGenerator* psgen;
    MidiGenerator* midigen;
    MidiGenerator autoplayer;
    SpectreDocument* spectre = nullptr;
    static const QRegularExpression m_abcext;
};
#endif
