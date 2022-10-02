#ifndef ABCPROCESS_H
#define ABCPROCESS_H

#include <QProcess>

class AbcProcess : public QProcess
{
    Q_OBJECT
public:
    enum ProcessType {ProcessUnknown, ProcessCompiler, ProcessPlayer};
    explicit AbcProcess(ProcessType which, QObject *parent, int cont);
    ProcessType which();
    QByteArray* log();

signals:
    void finished(int exitCode, QProcess::ExitStatus exitStatus, ProcessType which, int cont);
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
    int cont; /* wether or not continue in processing chain */
    QByteArray output;
};

#endif // ABCPROCESS_H
