#ifndef ABCSMF_H
#define ABCSMF_H

#include <QObject>
#include <smf.h>
#include "../abc/abc.h"

class AbcSmf : public QObject
{
    Q_OBJECT
public:
    explicit AbcSmf(struct abc* yy, int vel, int short_den, int x = 1, QObject *parent = nullptr);
    void reset();
    void writeAll();
    void saveToFile(const char* filename);
    ~AbcSmf();

private:
#define DPQN 960 /* divisions per quarter note */

    void manageDecoration(struct abc_symbol* s);
    void writeSingleNote(smf_track_t* track, char voice_nr, struct abc_symbol* s);
    void getNumDen(const char* text, long* num, long* den);
    int getSMFKeySignature(const char* text, int* mode);
    void setDynamic(long dur);
    void writeName(smf_track_t* track, const char* l);
    void writeLyric(smf_track_t *track, const char* l);
    void writeBpmTempo(smf_track_t* track, char val);
    void writeTimeSignature(smf_track_t *track, unsigned char numerator, unsigned char denominator);
    void writeKeySignature(smf_track_t *track, unsigned char keysig, unsigned char mode);
    void writeMidiEvent(smf_track_t* track, int delta, unsigned char ev_type, unsigned char ev_key, char ev_val);
    void writeMetaEvent(smf_track_t* track, int delta, char ev_type);
    void writeMetaEvent(smf_track_t* track, int delta, char ev_type, char ev_val);
    void writeMetaEvent(smf_track_t* track, int delta, char ev_type, const char* ev_val);
    void writeTrack(smf_track_t *t, int voice_nr);

    struct abc* m_yy; /* takes ownership of yy */
    int m_x;
    struct abc_tune* m_tune; /* tune selected by X */
    char* m_keysig;          /* tune's kh->text */
    int m_midi_keysig;       /* MIDI smf key signature */
    int m_midi_mode;         /* MIDI smf mode (maj/min) */

    const char* m_unit_length; /* L header text */
    const char* m_metric;      /* M header text */
    long m_tick_per_unit;      /* ticks per unit */
    long m_unit_per_whole;     /* units per whole note */
    long m_tempo;              /* quarter per minute */

    int m_emphasis;      /* per note temporary delta velocity */
    long m_last_tick;
    long m_note_dur;     /* note duration */
    int m_in_slur;
    int m_shorter;       /* [1-10] 1 is shortest, 10 is 100% duration */
    double m_grace_mod;  /* duration modified for grace notes */
    int m_in_cresc;
    unsigned char m_mark_dyn;
    unsigned char m_cur_dyn;
    long m_grace_tick;   /* ticks elapsed by a grace group */
    unsigned char m_noteon;
    unsigned char m_program;
    unsigned char m_control;
    int m_transpose;
    int m_default_velocity;
    int m_default_shorter;
    smf_t* m_smf;
};

#endif // ABCSMF_H
