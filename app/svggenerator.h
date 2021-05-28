#ifndef SVGGENERATOR_H
#define SVGGENERATOR_H

#include "psgenerator.h"
#include <QObject>

class SvgGenerator : public PsGenerator
{
    Q_OBJECT
public:
    SvgGenerator(QObject* parent = nullptr);
    void generate(const QString& input, int xopt, QString output, int cont) override;
};

#endif // SVGGENERATOR_H
