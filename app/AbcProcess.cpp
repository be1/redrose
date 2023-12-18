#include "AbcProcess.h"
#include <QDebug>

AbcProcess::AbcProcess(ProcessType which, QObject *parent, Continuation cont)
    : QProcess(parent),
      type(which),
      cont(cont)
{
    connect(this, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, &AbcProcess::onFinished);
#if 1
    //setProcessChannelMode(MergedChannels);
    connect(this, &QProcess::readyRead, this, &AbcProcess::onOutput);
#else
    setProcessChannelMode(SeparateChannels);
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
    stdlog = readAllStandardError();
    stdlog += readAllStandardOutput();
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
