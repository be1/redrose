#include "AbcProcess.h"
#include <QDebug>

AbcProcess::AbcProcess(ProcessType which, QObject *parent, Continuation cont)
    : QProcess(parent),
      type(which),
      cont(cont)
{
    setProcessChannelMode(SeparateChannels);
    connect(this, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, &AbcProcess::onFinished);
#if 1
    connect(this, &QProcess::readyRead, this, &AbcProcess::onOutput);
#else
    connect(this, &QProcess::readyReadStandardOutput, this, &AbcProcess::onStdout);
    connect(this, &QProcess::readyReadStandardError, this, &AbcProcess::onStderr);
#endif
}

AbcProcess::ProcessType AbcProcess::which()
{
    return type;
}

QByteArray *AbcProcess::log()
{
   return &stdlog;
}
#if 1
void AbcProcess::onOutput()
{
    stdlog = readAll();
    emit outputText(stdlog);
}
#else
void AbcProcess::onStdout()
{
    emit outputText(readAllStandardOutput());
    /*
    while(canReadLine()) {
        emit outputText(readLine());
    }
    */
}

void AbcProcess::onStderr()
{
   emit errorText(readAllStandardError());
}
#endif
void AbcProcess::onFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    emit finished(exitCode, exitStatus, type, this->cont);
}
