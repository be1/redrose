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

    bool fromAbcBuffer(const QByteArray &ba, bool with_charmap);
    bool selectTuneNo(int no);
    bool selectVoiceNo (int tune_no, int no);
    int charIndexFromMidiTick(long tick) const;
    long midiTickFromCharIndex(int uidx) const;
    int midiKeyFromCharIndex(int uidx) const;
    //int xvFromCharIndex(char XorV, int uidx) const;

    bool hasError() const;
    int errorLine() const;
    int errorChar() const;

    QByteArray abcBuffer() const;
    struct abc* implementation() const;

protected:
    abc_tune *tuneOfModel(int tune) const;
    void createCharMapping(); /* bytes to character mapping in the buffer */

private:
    int m_tune_no = 0;
    int m_voice_no = 0;

    abc_voice* m_voice_events = nullptr;

    QByteArray m_buffer;
    struct abc* m_implementation = nullptr;

    int* m_charmap = nullptr;
    int searchIdx(int *ordered, int siz, int needle) const;
};

#endif // ABCMODEL_H
