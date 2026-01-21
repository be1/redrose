#include <QDebug>
#include <QMessageBox>
#include <QRegularExpression>
#include "midigenerator.h"
#include "generator.h"
#include "settings.h"
#include "../abc/abc.h"
#include "abcsmf.h"
#include "config.h"

MidiGenerator::MidiGenerator(const QString &outfile, QObject *parent):
    Generator(outfile, parent)
{
}

MidiGenerator::MidiGenerator(const AbcModel *model, const QString &outfile, QObject *parent)
    : Generator(model, outfile, parent)
{
}

void MidiGenerator::generate(const QString &inputpath, int xopt, AbcProcess::Continuation cont)
{
    Settings settings;
    QVariant player = settings.value(PLAYER_KEY);

    QString program = player.toString();

    if (player == LIBABC2SMF) {
        /* use internal generator */
        generateInternal(inputpath, xopt, cont);
        return;
    }

    /* or use abc2midi */
    QStringList argv = program.split(" ");
    program = argv.at(0);

    if (outFile().isEmpty()) {
        QString path = inputpath;
        path.replace(m_abcext, QString::number(xopt) + ".mid");
        setOutFile(path);
    }

    argv.removeAt(0);
    argv << inputpath;
    argv << QString::number(xopt);
    argv << "-v";
    argv << "-o" << outFile();
    QFileInfo info(inputpath);
    QDir dir = info.absoluteDir();

    spawnMidiCompiler(program, argv, dir, cont);
}

/* this use internal MIDI to SMF generator */
void MidiGenerator::generateInternal(const QString &inputnamehint, int xopt, AbcProcess::Continuation cont)
{
    Settings settings;
    QVariant vel = settings.value(PLAYER_VELOCITY);
    QVariant dur = settings.value(PLAYER_DURATION);
    QVariant expr = settings.value(PLAYER_EXPRESSION);

    if (!vel.isValid())
        vel = 80;
    if (!dur.isValid())
        dur = 95;
    if (!expr.isValid())
        expr = false;

    QString errstr;
    if (model()->hasError()) {
        errstr = tr("Parse error line: ") + QString::number(model()->errorLine()) + tr(", char: ") + QString::number(model()->errorChar());
        emit generated(1, errstr, cont);
    } else {
        AbcSmf *smf = new AbcSmf(vel.toInt(), dur.toInt(), expr.toBool(), model(), this);
        if (!smf) {
            emit generated(1, tr("Out of memory"), cont);
            return;
        }

        if (!smf->select(xopt)) {
            delete smf;
            emit generated(0, tr("Tune does not exist"), cont);
            return;
        }

        if (outFile().isEmpty()) {
            QString path = inputnamehint;
            path.replace(m_abcext, QString::number(xopt) + ".mid");
            setOutFile(path);
        }

        smf->saveToFile(outFile().toUtf8().constData());

        delete smf;
        emit generated(0, "", cont);
    }
}
#if 0
const QDataStream* MidiGenerator::generate(const QByteArray &inputbuf, int xopt)
{
    struct abc* yy = abc_parse_buffer(inputbuf.constData(), inputbuf.size());
    if (yy->error) {
        abc_release_yy(yy);
        return nullptr;
    } else {
        AbcSmf *smf = new AbcSmf(yy, 80, 10, xopt, this);
        if (!smf) {
            abc_release_yy(yy);
            return nullptr;
        }

        QDataStream* stream = new QDataStream(new QByteArray(), QIODevice::ReadWrite);
        smf->writeToStream(stream);

        delete smf;
        abc_release_yy(yy);
        return stream;
    }
}
#endif
void MidiGenerator::spawnMidiCompiler(const QString& prog, const QStringList &args, const QDir &wrk, AbcProcess::Continuation cont)
{
    return spawnProgram(prog, args, AbcProcess::ProcessPlayer, wrk, cont);
}
