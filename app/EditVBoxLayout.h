#ifndef EDITVBOXLAYOUT_H
#define EDITVBOXLAYOUT_H

#include "RunPushButton.h"
#include "PlayPushButton.h"
#include "AbcPlainTextEdit.h"
#include "AbcProcess.h"
#include "AbcTemporaryFile.h"
#include "abcsynth.h"
#include "psgenerator.h"
#include "svggenerator.h"
#include "midigenerator.h"
#include "QProgressIndicator.h"
#include <QVBoxLayout>
#include <QSpinBox>
#include <QLabel>
#include <QDir>
#include <QTemporaryFile>
#include <QThread>

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
    void exportPostscript(QString filename);
    /**
     * @brief exportMIDI
     * @param filename Output file path. Can be emtpy (has defaults).
     */
    void exportMIDI(QString filename);

signals:
    void doExportMIDI(const QString& outfilename);

protected:
    void spawnProgram(const QString& prog, const QStringList &args, AbcProcess::ProcessType which, const QDir &wrk, int cont);
    void removeSvgFiles();
    void removeMIDIFile();
    void playMIDI();
    int xOfCursor(const QTextCursor& c);
    void cleanupProcesses();
    void cleanupThreads();

public slots:
    void onXChanged(int value);
    void onPlayClicked(); /* midi */
    void onRunClicked(); /* svg */
    void onSelectionChanged();

protected slots:
    void onCompileFinished(int exitCode, const QString& errstr, int cont);
    void onGenerateMIDIFinished(int exitCode, const QString& errstr, int cont);
    void onSynthInited(bool err);
    void onSynthFinished(bool err);

private:
	AbcPlainTextEdit abcplaintextedit;
    PlayPushButton playpushbutton; /* midi */
    RunPushButton runpushbutton; /* svg */
    QHBoxLayout hboxlayout;
    QSpinBox xspinbox;
    QLabel xlabel;
    QString fileName;
    AbcTemporaryFile tempFile;
    QList<AbcProcess*> processlist;
    QProgressIndicator* progress;
    QString selection;
    int selectionIndex;

    AbcSynth* synth;
    PsGenerator psgen;
    SvgGenerator svggen;
    MidiGenerator midigen;
};
#endif
