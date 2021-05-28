#ifndef PSGENERATOR_H
#define PSGENERATOR_H

#include "generator.h"
#include <QObject>

class PsGenerator : public Generator
{
    Q_OBJECT
public:
    explicit PsGenerator(QObject* parent = nullptr);
    void generate(const QString& input, int xopt, QString output, int cont) override;

protected:
    void spawnPsCompiler(const QString &prog, const QStringList& args, const QDir &wrk, int cont);
};

#endif // PSGENERATOR_H
