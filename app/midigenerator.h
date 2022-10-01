#ifndef MIDIGENERATOR_H
#define MIDIGENERATOR_H

#include "generator.h"
#include <QObject>
#include <QByteArray>

class MidiGenerator : public Generator
{
    Q_OBJECT
public:
    MidiGenerator(QObject* parent = nullptr);
    void generate(const QString& input, int xopt, QString output, int cont) override;
    /**
     * @brief generate.
     * @param inputbuf Input buffer.
     * @param inpputhint Input filename hint.
     * @param xopt Tune selection.
     * @param output Output filename.
     * @param cont Unused, forwarded.
     */
    void generate(const QByteArray& inputbuf, const QString& inpputhint, int xopt, QString output, int cont);

    const QDataStream* generate(const QByteArray& inputbuf, int xopt);
protected:
    void spawnMidiCompiler(const QString &prog, const QStringList& args, const QDir &wrk, int cont);

};

#endif // MIDIGENERATOR_H
