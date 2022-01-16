#include <QRegularExpression>
#include <QDebug>
#include "svggenerator.h"
#include "config.h"
#include "../abcm2ps/abcm2ps.h"
#include "settings.h"

SvgGenerator::SvgGenerator(QObject* parent)
    : PsGenerator(parent)
{

}

void SvgGenerator::generate(const QString &input, int xopt, QString output, int cont)
{
    Settings settings;
#ifndef USE_LIBABCM2PS
    QVariant compiler = settings.value(COMPILER_KEY);
    QString program = compiler.toString();
#else
    QString program("abcm2ps");
#endif
    QStringList argv = program.split(" ");
    program = argv.at(0);

    argv << "-v"; /* SVG output*/
    argv << "-e" << QString::number(xopt); /* one tune to consider */
    argv << "-N1"; /* numbering pages on the up-left */
    if (output.isEmpty()) {
        output = input;
        output.replace(QRegularExpression("\\.abc$"), ".svg"); /* but remind, output will be tempNNN.svg */
    }
    argv << "-O" << output; /* output filename */
    argv << input; /* input ABC */

    QFileInfo info(input);
    QDir dir = info.absoluteDir();
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

    if (ret)
        emit generated(ret, tr("Error during score generation."), cont);
    else
        emit generated(ret, "", cont);
#else
    argv.removeAt(0);
    spawnPsCompiler(program, argv, dir, cont);
#endif
}
