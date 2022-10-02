#ifndef GENERATOR_H
#define GENERATOR_H

#include <QObject>
#include <QDir>
#include "AbcProcess.h"

class Generator : public QObject
{
    Q_OBJECT
public:
    explicit Generator(QObject *parent = nullptr);
    /**
     * @brief generate
     * @param input filename to process.
     * @param xopt X tune to process.
     * @param output filename to output to. If empty, has defaults.
     * @param cont play/display flag, unused, forwarded.
     */
    virtual void generate(const QString& input, int xopt, QString output, int cont) = 0;
    /**
     * @brief generate the first MIDI key of abcbuf
     * @param abcbuf the abc score as a string
     * @attention this function works in foreground and no signel is triggered.
     */
    bool genFirstNote(const QString& abcbuf, int* chan, int* pgm, int* key);

signals:
    void generated(bool err, const QString& errstr, int cont);

protected:
    void spawnProgram(const QString& prog, const QStringList& args, AbcProcess::ProcessType which, const QDir& wrk, int cont);

protected slots:
    void onProgramFinished(int exitCode, QProcess::ExitStatus exitStatus, AbcProcess::ProcessType, int cont);
private:
    QList<AbcProcess*> processlist;
    /**
     * @brief getError
     * @param form process output
     * @param to formatted error line
     * @return pseudo exitCode
     * @attention this works only for abc2midi
     */
    int getError(const QString& form, QString* to = nullptr);
};

#endif // GENERATOR_H
