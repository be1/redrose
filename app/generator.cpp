#include <QDebug>
#include <QRegularExpression>

#include "generator.h"
#include "AbcProcess.h"
#include "../abc/abc.h"

const QRegularExpression Generator::m_abcext("\\.abc$");

Generator::Generator(const AbcModel* model, const QString &outfile, QObject *parent) :
    QObject(parent),
    m_model(model),
    outfile(outfile)
{
}

Generator::Generator(const QString &outfile, QObject *parent):
    QObject(parent),
    outfile(outfile)
{
}

const QString &Generator::outFile()
{
    return outfile;
}

void Generator::setOutFile(const QString &outfile)
{
    this->outfile = outfile;
}

bool Generator::genFirstNote(const QString &abcbuf, int* chan, int* pgm, int* key)
{
    QByteArray ba = abcbuf.toUtf8();
    struct abc* abc = abc_parse_buffer(ba.constData(), ba.size());

    if (!abc->count)
        return false;

    if (!abc->tunes[0]->count)
        return false;

    /* find MIDI channel and program... */
    int v = 0, p = 0, t = 0;
    if (abc->tunes[0]->voices[0]->v &&
            QChar::isDigit(abc->tunes[0]->voices[0]->v[0]))
        v = atoi(abc->tunes[0]->voices[0]->v) -1;

    struct abc_symbol* sym = abc->tunes[0]->voices[0]->first;
    if (!sym) {
        abc_release_yy(abc);
        return false;
    }

    while (sym && sym->kind != ABC_NOTE) {
        if (sym->kind == ABC_INST) {
            int val;
            if (sscanf(sym->text, "MIDI channel %d", &val)) {
                v = val -1; /* NOTE: -1 */
            }

            if (sscanf(sym->text, "MIDI program %d", &val)) {
                p = val;
            }

            if (sscanf(sym->text, "MIDI transpose %d", &val)) {
                t = val;
            }
        }

        sym = sym->next;
    }

    /* MIDI key */
    if (sym) {
        if (key)
            *key = sym->ev.key + t;
        if (chan)
            *chan = v;
        if (pgm)
            *pgm = p;

        abc_release_yy(abc);
        return true;
    }

    abc_release_yy(abc);
    return false;
}

void Generator::spawnProgram(const QString& prog, const QStringList& args, AbcProcess::ProcessType which, const QDir& wrk, AbcProcess::Continuation cont)
{
    AbcProcess *process = new AbcProcess(which, this, cont);
    process->setWorkingDirectory(wrk.absolutePath());
    connect(process, QOverload<int, QProcess::ExitStatus, AbcProcess::ProcessType, AbcProcess::Continuation>::of(&AbcProcess::finished), this, &Generator::onProgramFinished);
    connect(process, &AbcProcess::errorOccurred, this, &Generator::onProgramError);
    processlist.append(process);
    qDebug() << prog << args;
    process->start(prog, args);
}

const AbcModel *Generator::model() {
    return m_model;
}

void Generator::onProgramFinished(int exitCode, QProcess::ExitStatus exitStatus, AbcProcess::ProcessType which, AbcProcess::Continuation cont)
{
    /* delete garbage */
    QString output;
    for (int i = 0; i < processlist.length(); i++) {
        AbcProcess* proc = processlist.at(i);
        if (proc->state() == QProcess::NotRunning
                && proc->exitCode() == exitCode
                && proc->exitStatus() == exitStatus
                && proc->which() == which) {
            output = QString::fromUtf8(*(proc->log()));
            disconnect(proc, QOverload<int, QProcess::ExitStatus, AbcProcess::ProcessType, AbcProcess::Continuation>::of(&AbcProcess::finished), this, &Generator::onProgramFinished);
            delete proc;
            processlist.removeAt(i);
        }
    }

    emit generated(exitCode, output, cont);
}

void Generator::onProgramError(QProcess::ProcessError err)
{
    AbcProcess* p = qobject_cast<AbcProcess*>(sender());
    qWarning() << p->program() << p->errorString();
    if (err == QProcess::FailedToStart) {
        emit generated(127, p->program() + ": " + p->errorString(), AbcProcess::ContinuationNone);
    }
}

int Generator::getGenerationError(const QString& from, QString* to)
{
    int ret = 0;
    if (from.contains("Error"))
        ret = 1;

    int beg, end;
    beg = from.lastIndexOf("Error");
    end = from.indexOf('\n', beg);
    if (to) {
        to->clear();
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
        to->append(from.midRef(beg, end - beg));
#else
        to->append(from.mid(beg, end - beg));
#endif
    }

    return ret;
}
