#include <QDebug>
#include "psgenerator.h"
#include "config.h"
#ifdef USE_LIBABCM2PS
#include "../abcm2ps/abcm2ps.h"
#endif
#include "AbcApplication.h"
#include "AbcProcess.h"
#include "settings.h"

PsGenerator::PsGenerator(const QString& outfile, QObject* parent)
    : Generator(outfile, parent)
{
}

void PsGenerator::generate(const QString &input, int xopt, AbcProcess::Continuation cont)
{
    Settings settings;
    QVariant param = settings.value(PSTUNES_KEY);

    QString program("abcm2ps");
    QStringList argv = program.split(" ");

    if (outFile().isEmpty()) {
        QString path = input;
        path.replace(m_abcext, ".ps");
        setOutFile(path);
    }

    if (param.toString() == TUNES_ALL) {
        argv << "-N1" << input << "-O" << outFile();
    } else {
        argv << "-N1" << input << "-e" << QString::number(xopt) << "-O" << outFile();
    }

#ifdef USE_LIBABCM2PS
    QString s;
    QByteArray ba;
    char **av = (char**)malloc(argv.length() * sizeof (char*));
    for (int i = 0; i < argv.length(); i++) {
        s = argv.at(i);
        ba = s.toUtf8();
        av[i] = (char*)malloc((ba.length() + 1) * sizeof (char));
        strncpy(av[i], ba.constData(), ba.length());
        av[i][ba.length()] = '\0';
    }

    int ret = abcm2ps(argv.length(), av);

    for (int i = 0; i < argv.length(); i++) {
        free(av[i]);
    }
    free(av);
    if (ret) {
        emit generated(ret, tr("Error during score generation."), cont);
    } else {
        emit generated(ret, "", cont);
    }
#else
    QFileInfo info(input);
    QDir dir = info.absoluteDir();
    argv.removeAt(0);
    spawnPsCompiler(program, argv, dir, cont);
#endif
}


void PsGenerator::spawnPsCompiler(const QString &prog, const QStringList& args, const QDir &wrk, AbcProcess::Continuation cont)
{
    return spawnProgram(prog, args, AbcProcess::ProcessCompiler, wrk, cont);
}
