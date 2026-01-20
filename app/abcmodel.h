#ifndef ABCMODEL_H
#define ABCMODEL_H

#include <QByteArray>
#include "abc.h"

#define DPQN 480 /* divisions per quarter note */

class AbcModel
{
public:
    AbcModel();
    ~AbcModel();

    bool fromAbcBuffer(const QByteArray& ba);
    void selectTuneNo (int no);
    void selectVoiceNo (int tune_no, int no);

    void getNumDen(const char* text, long* num, long* den) const;
    long ticksPerUnit(abc_tune *tu) const;
    long unitsPerWhole(abc_tune *tu) const;

    int charIndexFromAbcEvent(int num, int den) const;
    int symbolIndexFromAbcEvent(int num, int den) const;

    int charIndexFromMidiTick(int tick) const;
    //int symbolIndexFromMidiTick(int tune, int voice, int tick) const;

    //int midiTickFromCharIndex(int cidx) const;

    bool hasError() const;
    int errorLine() const;
    int errorChar() const;

    QByteArray abcBuffer() const;
    struct abc* implementation() const;

protected:
    abc_tune *tuneOfModel(int tune) const;
    abc_voice *voiceOfTune(int tune, int voice) const;
    void createCharMapping(); /* bytes to character mapping in the buffer */

private:
    int m_tune_no = 0;
    int m_voice_no = 0;
    long m_ticks_per_unit = 0;
    long m_units_per_whole = 0;

    abc_tune* m_tune = nullptr;
    abc_voice* m_voice = nullptr;
    abc_voice* m_voice_events = nullptr;

    QByteArray m_buffer;
    struct abc* m_implementation = nullptr;

    int* m_charmap = nullptr;
};

#endif // ABCMODEL_H
