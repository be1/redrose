#ifndef MIDIGENERATOR_H
#define MIDIGENERATOR_H

#include "generator.h"
#include "abcmodel.h"
#include <QObject>
#include <QByteArray>

class MidiGenerator : public Generator
{
    Q_OBJECT
public:

    MidiGenerator(const QString& outfile = "", QObject* parent = nullptr);
    MidiGenerator(const AbcModel* model, const QString& outfile = "", QObject* parent = nullptr);
    /**
     * @brief generate.
     * @param inpputhint Input filename hint.
     * @param xopt Tune selection.
     * @param output Output filename.
     * @param cont Unused, forwarded.
     */
    void generate(const QString& inputpath, int xopt, AbcProcess::Continuation cont) override;
#if 0
    /* DON'T USE THIS: */
    const QDataStream* generate(const QByteArray& inputbuf, int xopt);
#endif

protected:
    void generateInternal(const QString& inputnamehint, int xopt, AbcProcess::Continuation cont);
    void spawnMidiCompiler(const QString& prog, const QStringList& args, const QDir& wrk, AbcProcess::Continuation cont);

};

#endif // MIDIGENERATOR_H
