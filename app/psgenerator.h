#ifndef PSGENERATOR_H
#define PSGENERATOR_H

#include "generator.h"
#include <QObject>

class PsGenerator : public Generator
{
    Q_OBJECT
public:
    explicit PsGenerator(const QString& outfile, QObject* parent = nullptr);
    void generate(const QString& input, int xopt, AbcProcess::Continuation cont) override;

protected:
    void spawnPsCompiler(const QString& prog, const QStringList& args, const QDir& wrk, AbcProcess::Continuation cont);
};

#endif // PSGENERATOR_H
