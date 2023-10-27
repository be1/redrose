#ifndef ABCPROCESS_H
#define ABCPROCESS_H

#include <QProcess>

class AbcProcess : public QProcess
{
    Q_OBJECT
public:
    enum ProcessType {ProcessUnknown, ProcessCompiler, ProcessPlayer};
    enum Continuation {ContinuationNone, ContinuationRender, ContinuationConvert};

    explicit AbcProcess(ProcessType which, QObject *parent, Continuation cont);
    ProcessType which();
    QByteArray* log();

signals:
    void finished(int exitCode, QProcess::ExitStatus exitStatus, AbcProcess::ProcessType which, AbcProcess::Continuation cont);
    void outputText(const QByteArray& text);
#if 0
    void errorText(const QByteArray& text);
#endif

protected slots:
#if 1
    void onOutput();
#else
    void onStdout();
    void onStderr();
#endif
    void onFinished(int exitCode, QProcess::ExitStatus exitStatus);

private:
    ProcessType type;
    Continuation cont; /* wether or not continue in processing chain */
    QByteArray stdlog; /* stderr/out buffer */
};

#endif // ABCPROCESS_H
