#include <QDebug>
#include <QMessageBox>
#include "AbcApplication.h"
#include "midigenerator.h"
#include "generator.h"
#include "settings.h"
#include "../abc/abc.h"
#include "abcsmf.h"

MidiGenerator::MidiGenerator(QObject* parent)
    : Generator(parent)
{

}

void MidiGenerator::generate(const QString &input, int xopt, QString output, int cont)
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

    if (output.isEmpty()) {
        output = input;
        output.replace(QRegularExpression("\\.abc$"), QString::number(xopt) + ".mid");
    }

    argv.removeAt(0);
    argv << input;
    argv << QString::number(xopt);
    argv << "-v";
    argv << "-o" << output;
    QFileInfo info(input);
    QDir dir = info.absoluteDir();

    spawnMidiCompiler(program, argv, dir, cont);
}

void MidiGenerator::generate(const QByteArray &inputbuf, const QString& inputhint, int xopt, QString output, int cont)
{
    QString errstr;
    struct abc* yy = abc_parse_buffer(inputbuf.constData(), inputbuf.count());
    if (yy->error) {
        errstr = tr("Parse error line: ") + QString::number(yy->error_line) + tr(", char: ") + QString::number(yy->error_char);
        emit generated(1, errstr, cont);
        abc_release_yy(yy);
    } else {
        AbcSmf *smf = new AbcSmf(yy, xopt, this);
        if (!smf) {
                emit generated(1, tr("Out of memory"), cont);
                abc_release_yy(yy);
                return;
        }

        if (output.isEmpty()) {
                output = inputhint;
                output.replace(QRegularExpression("\\.abc$"), QString::number(xopt) + ".mid");
        }

        smf->writeToFile(output);

        delete smf;
        emit generated(0, "", cont);
        abc_release_yy(yy);
    }
}

void MidiGenerator::spawnMidiCompiler(const QString& prog, const QStringList &args, const QDir &wrk, int cont)
{
    AbcApplication* a = static_cast<AbcApplication*>(qApp);
    AbcMainWindow *w = a->mainWindow();
    w->mainHSplitter()->viewWidget()->logView()->clear();
    return spawnProgram(prog, args, AbcProcess::ProcessPlayer, wrk, cont);
}
