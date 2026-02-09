#ifndef ABCSMF_H
#define ABCSMF_H

#include <QObject>
#include <smf.h>
#include "../abc/abc.h"
#include "abcmodel.h"

class AbcSmf : public QObject
{
    Q_OBJECT
public:
    explicit AbcSmf(int vel = 96, int shorter = 9, bool expr = 0, const AbcModel* model = nullptr, QObject* parent = nullptr);
    bool select(int x = 1);
    void reset();
    void writeAll();
    void saveToFile(const char* filename);
    ~AbcSmf();

private:
    bool isDynamicMark(const struct abc_symbol* deco);
    bool isDynamicEnd(const abc_symbol* deco);
    char getDynamic(const char* mark, unsigned char base, unsigned char dflt);
    void applyDecoration(struct abc_symbol* s);
    void writeSingleNoteEvent(smf_track_t* track, unsigned char voice_nr, struct abc_symbol* s);
    void getNumDen(const char* text, long* num, long* den);
    int getSMFKeySignature(const char* text, int* mode);
    unsigned char computeNextVelocity(long dur, unsigned char cur);
    unsigned char getDynAfter(struct abc_symbol* note, unsigned char base, unsigned char dflt);
    int writeExpression (smf_track_t* track, long delta, unsigned char chan, unsigned char value);
    void parseVoiceHeader(const char* v);
    void writeName(smf_track_t* track, const char* l);
    void writeLyric(smf_track_t* track, const char* l);
    void writeBpmTempo(smf_track_t* track, long val);
    void writeTimeSignature(smf_track_t* track, unsigned char numerator, unsigned char denominator);
    void writeKeySignature(smf_track_t* track, unsigned char keysig, unsigned char mode);
    void writeMidiEvent(smf_track_t* track, long int delta, unsigned char ev_type, unsigned char ev_key, unsigned char ev_val);
    void writeMidiEvent(smf_track_t* track, long int delta, unsigned char ev_type);
    void writeMidiEvent(smf_track_t* track, long int delta, unsigned char ev_type, unsigned char ev_val);
    void writeMidiEvent(smf_track_t* track, long int delta, unsigned char ev_type, const char* ev_val);
    void writeTrack(smf_track_t* t, int voice_nr);

    struct abc_symbol* findClosingDynamics(struct abc_symbol* s, long* duration);
    long computeCut(struct abc_symbol* s);
    void generateExpressionArray(unsigned char climax);

    struct abc* m_yy = nullptr; /* unowned */
    int m_x = 0;
    QString m_inst_name;     /* given in V: header */
    struct abc_tune* m_tune = nullptr; /* tune selected by X */
    char* m_keysig;          /* tune's kh->text */
    int m_midi_keysig;       /* MIDI smf key signature */
    int m_midi_mode;         /* MIDI smf mode (maj/min) */

    const char* m_unit_length; /* L header text */
    const char* m_metric;      /* M header text */
    long m_tick_per_unit;      /* ticks per unit */
    long m_unit_per_whole;     /* units per whole note */
    long m_tempo;              /* quarter per minute */

    QList<QPair<long, unsigned char>> m_expression_array; /* list of <tick, value> */
    const unsigned char m_expression_ticks = 24;
    const unsigned char m_minimum_expression = 40;
    const unsigned char m_default_expression = 80;
    unsigned char m_cur_expression = 0;
    int m_first_note_in_cresc = 0;
    long m_cresc_duration = 0;
    struct abc_symbol* m_last_note_in_cresc = nullptr;
    int m_emphasis;      /* per note temporary delta velocity */
    long m_last_tick;
    int m_in_slur;
    long m_shorter;       /* [1-10] 1 is shortest, 10 is 100% duration */
    double m_grace_mod;  /* duration modified for grace notes */
    int m_in_cresc;
    unsigned char m_mark_dyn = 80;
    unsigned char m_cur_dyn = 0;
    long m_grace_tick;   /* ticks elapsed by a grace group */
    const unsigned char m_noteon = 0x90;
    const unsigned char m_program = 0xc0;
    const unsigned char m_control = 0xb0;
    long m_transpose;
    unsigned char m_default_velocity;
    unsigned char m_minimum_velocity = 40;
    unsigned char m_climax_velocity = 127; /* can be pppp or ffff */
    long m_default_shorter;
    int m_chord_cc_done = 0;
    bool m_manage_expression = false;
    const AbcModel* m_model; /* unowned */
    smf_t* m_smf;
};

#endif // ABCSMF_H
