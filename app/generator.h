#ifndef GENERATOR_H
#define GENERATOR_H

#include <QObject>
#include <QDir>
#include "AbcProcess.h"

class Generator : public QObject
{
    Q_OBJECT
public:
    explicit Generator(const QString& outfile = "", QObject *parent = nullptr);
    /**
     * @brief generate
     * @param input filename to process.
     * @param xopt X tune to process.
     * @param cont play/display flag, unused, forwarded.
     */
    virtual void generate(const QString& input, int xopt, AbcProcess::Continuation cont) = 0;
    /**
     * @brief generate the first MIDI key of abcbuf
     * @param abcbuf the abc score as a string
     * @attention this function works in foreground and no signel is triggered.
     */
    const QString& outFile();
    void setOutFile(const QString& outfile);

    bool genFirstNote(const QString& abcbuf, int* chan, int* pgm, int* key);

signals:
    void generated(bool err, const QString& errstr, AbcProcess::Continuation cont);

protected:
    void spawnProgram(const QString& prog, const QStringList& args, AbcProcess::ProcessType which, \
                      const QDir& wrk, AbcProcess::Continuation cont);
    static const QRegularExpression m_abcext;

protected slots:
    void onProgramFinished(int exitCode, QProcess::ExitStatus exitStatus, AbcProcess::ProcessType, AbcProcess::Continuation cont);
    /**
     * @brief onProgramError slot on program not found or bad binary
     * @param err the QProcess error number
     */
    void onProgramError(QProcess::ProcessError err);
private:
    QList<AbcProcess*> processlist;
    /**
     * @brief getGenerationError formats the error string of generator error text
     * @param from process output
     * @param to formatted error line
     * @return pseudo exitCode
     * @attention this works only for abc2midi
     */
    int getGenerationError(const QString& from, QString* to = nullptr);
    QString outfile;
};

#endif // GENERATOR_H
