#ifndef MIDIGENERATOR_H
#define MIDIGENERATOR_H

#include "generator.h"
#include <QObject>
#include <QByteArray>

class MidiGenerator : public Generator
{
    Q_OBJECT
public:
    explicit MidiGenerator(const QString &outfile = "", QObject* parent = nullptr);
    void generate(const QString& inputpath, int xopt, AbcProcess::Continuation cont) override;
    /**
     * @brief generate.
     * @param inputbuf Input buffer.
     * @param inpputhint Input filename hint.
     * @param xopt Tune selection.
     * @param output Output filename.
     * @param cont Unused, forwarded.
     */
    void generate(const QByteArray& inputbuf, const QString& inpputhint, int xopt, AbcProcess::Continuation cont);

    /* DON't USE THIS: */
    const QDataStream* generate(const QByteArray& inputbuf, int xopt);

protected:
    void spawnMidiCompiler(const QString &prog, const QStringList& args, const QDir &wrk, AbcProcess::Continuation cont);

};

#endif // MIDIGENERATOR_H
