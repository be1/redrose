#ifndef ABCSMF_H
#define ABCSMF_H

#include <QObject>
#include <drumstick/qsmf.h>
#include "../abc/abc.h"

class AbcSmf : public drumstick::File::QSmf
{
    Q_OBJECT
public:
    explicit AbcSmf(struct abc* yy, int x = 1, QObject *parent = nullptr);

private slots:
    void onSMFWriteTempoTrack(void);
    void onSMFWriteTrack(int track);

private:
#define DPQN 960
#define DYN_DEFAULT 80
#define SHORTEN_DEFAULT 10
#define EXPRESSION_DEFAULT 0

    void manageDecoration(struct abc_symbol* s);
    void writeSingleNote(int track, struct abc_symbol* s);
    void getNumDen(const char* text, long* num, long* den);
    int getSMFKeySignature(const char* text, int* mode);
    void setDynamic(long dur);
    void writeExpression(int track);
    void writeLyric(const char* l);
    void feedPitchDiff(void);

    struct abc* yy; /* takes ownership of yy */
    int x;
    struct abc_tune* t;     /* tune selected by X */
    char* ks;               /* tune's kh->text */
    int mks;                /* MIDI smf key signature */
    int mode;               /* MIDI smf mode (maj/min) */

    const char* l; /* L header text */
    const char* m; /* M header text */
    long tpu;      /* ticks per unit */
    long upm;      /* units per measure */
    long tempo;    /* quarter per minute */

    int expr;      /* expression */
    long last_tick;
    long dur;      /* note duration */
    int in_slur;
    int shorten;
    double grace_mod; /* duration modified for grace notes */
    int in_cresc;
    unsigned char mark_dyn;
    unsigned char cur_dyn;
    long grace_tick; /* ticks elapsed by a grace group */
    char noteon;
    char program;
    char control;
    int transpose;
};

#endif // ABCSMF_H
