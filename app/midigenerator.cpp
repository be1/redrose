#include <QDebug>
#include <QMessageBox>
#include <QRegularExpression>
#include "midigenerator.h"
#include "generator.h"
#include "settings.h"
#include "../abc/abc.h"
#include "abcsmf.h"
#include "config.h"

MidiGenerator::MidiGenerator(const QString& outfile, QObject* parent)
    : Generator(outfile, parent)
{

}

void MidiGenerator::generate(const QString &inputpath, int xopt, AbcProcess::Continuation cont)
{
    Settings settings;
    QVariant player = settings.value(PLAYER_KEY);

    QString program = player.toString();
    QStringList argv = program.split(" ");
    program = argv.at(0);

    if (!QFileInfo::exists(program)) {
        emit generated(1, tr("Program not found: ") + program, cont);
        return;
    }

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

void MidiGenerator::generate(const QByteArray &inputbuf, const QString& inputhint, int xopt, AbcProcess::Continuation cont)
{
    Settings settings;
    QVariant vel = settings.value(PLAYER_VELOCITY);
    QVariant dur = settings.value(PLAYER_DURATION);

    if (!vel.isValid())
        vel = 80;
    if (!dur.isValid())
        dur = 10;

    QString errstr;
    struct abc* yy = abc_parse_buffer(inputbuf.constData(), inputbuf.count());
    if (yy->error) {
        errstr = tr("Parse error line: ") + QString::number(yy->error_line) + tr(", char: ") + QString::number(yy->error_char);
        emit generated(1, errstr, cont);
        abc_release_yy(yy);
    } else {
        AbcSmf *smf = new AbcSmf(yy, vel.toInt(), dur.toInt(), xopt, this);
        if (!smf) {
                emit generated(1, tr("Out of memory"), cont);
                abc_release_yy(yy);
                return;
        }

        if (outFile().isEmpty()) {
            QString path = inputhint;
            path.replace(m_abcext, QString::number(xopt) + ".mid");
            setOutFile(path);
        }

        smf->writeToFile(outFile());

        delete smf;
        emit generated(0, "", cont);
        abc_release_yy(yy);
    }
}

const QDataStream* MidiGenerator::generate(const QByteArray &inputbuf, int xopt)
{
    struct abc* yy = abc_parse_buffer(inputbuf.constData(), inputbuf.count());
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

void MidiGenerator::spawnMidiCompiler(const QString& prog, const QStringList &args, const QDir &wrk, AbcProcess::Continuation cont)
{
    return spawnProgram(prog, args, AbcProcess::ProcessPlayer, wrk, cont);
}
