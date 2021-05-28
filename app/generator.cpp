#include "generator.h"
#include "AbcProcess.h"
#include "AbcApplication.h"
#include <QDebug>

Generator::Generator(QObject *parent) : QObject(parent)
{
}

void Generator::spawnProgram(const QString& prog, const QStringList& args, AbcProcess::ProcessType which, const QDir& wrk, int cont)
{
    AbcProcess *process = new AbcProcess(which, this, cont);
    process->setWorkingDirectory(wrk.absolutePath());
    connect(process, QOverload<int, QProcess::ExitStatus, AbcProcess::ProcessType, int>::of(&AbcProcess::finished), this, &Generator::onProgramFinished);
    connect(process, &AbcProcess::outputText, this, &PsGenerator::onProgramOutputText);
    connect(process, &AbcProcess::errorText, this, &PsGenerator::onProgramErrorText);
    processlist.append(process);
    qDebug() << prog << args;
    process->start(prog, args);
}

void Generator::onProgramFinished(int exitCode, QProcess::ExitStatus exitStatus, AbcProcess::ProcessType which, int cont)
{
    /* delete garbage */
    QString errstr;
    for (int i = 0; i < processlist.length(); i++) {
        AbcProcess* proc = processlist.at(i);
        if (proc->state() == QProcess::NotRunning
                && proc->exitCode() == exitCode
                && proc->exitStatus() == exitStatus
                && proc->which() == which) {
            errstr = proc->errorString();
            disconnect(proc, QOverload<int, QProcess::ExitStatus, AbcProcess::ProcessType, int>::of(&AbcProcess::finished), this, &Generator::onProgramFinished);
            delete proc;
            processlist.removeAt(i);
        }
    }

    emit generated(exitCode != 0, errstr, cont);
}

void Generator::onProgramOutputText(const QByteArray &text)
{
    AbcApplication* a = static_cast<AbcApplication*>(qApp);
    AbcMainWindow* w =  a->mainWindow();
    LogView* lv = w->mainHSplitter()->viewWidget()->logView();
    lv->appendHtml("<em>" + QString::fromUtf8(text).replace("\n", "<br />") + "</em>");
}

void Generator::onProgramErrorText(const QByteArray &text)
{
    AbcApplication* a = static_cast<AbcApplication*>(qApp);
    AbcMainWindow* w =  a->mainWindow();
    LogView* lv = w->mainHSplitter()->viewWidget()->logView();
    lv->appendHtml("<b style=\"color: red\">" + QString::fromUtf8(text).replace("\n", "<br />") + "</b>");
}
