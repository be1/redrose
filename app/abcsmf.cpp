#include <QDebug>
#include <QTextCodec>
#include <drumstick/qsmf.h>
#include "abcsmf.h"

AbcSmf::AbcSmf(struct abc* yy, int x, QObject *parent) : drumstick::File::QSmf(parent),
        yy(yy),
        x(x),
        t(nullptr),
        ks(nullptr),
        l(nullptr),
        m(nullptr),
        tpu(0),
        upm(0),
        tempo(0),
        expr(EXPRESSION_DEFAULT),
        last_tick(0),
        dur(0),
        in_slur(SHORTEN_DEFAULT),
        in_cresc(0),
        mark_dyn(DYN_DEFAULT),
        cur_dyn(DYN_DEFAULT),
        grace_tick(0),
        noteon(0x90),
        program(0xc0),
        control(0xb0),
        transpose(0)
{
    connect(this, &QSmf::signalSMFWriteTempoTrack, this, &AbcSmf::onSMFWriteTempoTrack);
    connect(this, &QSmf::signalSMFWriteTrack, this, &AbcSmf::onSMFWriteTrack);

    t = abc_find_tune(yy, x);
    if (!t)
        return;

    struct abc_header* lh = abc_find_header(t, 'L');
    if (lh)
        l = lh->text;
    else
        l = "1/8";
    struct abc_header* mh = abc_find_header(t, 'M');
    if (mh)
        m = mh->text;
    else
        m = "4/4";

    upm = abc_unit_per_measure(l, m);
    struct abc_header* qh = abc_find_header(t, 'Q');
    if (qh)
        tempo = abc_tempo(qh->text);
    else
        tempo = 120;

    setDivision(DPQN);
    setTextCodec(QTextCodec::codecForName("UTF-8"));
    setFileFormat(1);
    setTracks(t->count);

    long num = 1, den = 8;
    getNumDen(l, &num, &den);

    tpu = (num * 4 * DPQN) / (den);

    struct abc_header* kh = abc_find_header(t, 'K');
    if (!kh) {
        ks = nullptr, mode = 0;
    } else {
        ks = kh->text;
        mks = getSMFKeySignature(ks, &mode);
    }
}

void AbcSmf::manageDecoration(struct abc_symbol* s) {
    if (!strcmp(s->text, "pppp")) mark_dyn = cur_dyn = 30;
    else if (!strcmp(s->text, "ppp")) mark_dyn = cur_dyn = 30;
    else if (!strcmp(s->text, "pp")) mark_dyn = cur_dyn = 45;
    else if (!strcmp(s->text, "p")) mark_dyn = cur_dyn = 60;
    else if (!strcmp(s->text, "mp")) mark_dyn = cur_dyn = 75;
    else if (!strcmp(s->text, "mf")) mark_dyn = cur_dyn = 90;
    else if (!strcmp(s->text, "f")) mark_dyn = cur_dyn = 105;
    else if (!strcmp(s->text, "ff")) mark_dyn = cur_dyn = 120;
    else if (!strcmp(s->text, "fff")) mark_dyn = cur_dyn = 127;
    else if (!strcmp(s->text, "ffff")) mark_dyn = cur_dyn = 127;
    else if (!strcmp(s->text, "sfz")) mark_dyn = cur_dyn = 100;
    else if (!strcmp(s->text, ".")) shorten = 2;
    else if (!strcmp(s->text, "H")) shorten = 100;
    else if (!strcmp(s->text, "tenuto")) shorten = 100;
    else if (!strcmp(s->text, "L")) expr = 127;
    else if (!strcmp(s->text, "accent")) expr = 127;
    else if (!strcmp(s->text, "emphasis")) expr = 127;
    else if (!strcmp(s->text, "crescendo(")) in_cresc = 1;
    else if (!strcmp(s->text, "<(")) in_cresc = 1;
    else if (!strcmp(s->text, "crescendo)")) in_cresc = 0;
    else if (!strcmp(s->text, "<)")) in_cresc = 0;
    else if (!strcmp(s->text, "diminuendo(")) in_cresc = -1;
    else if (!strcmp(s->text, ">(")) in_cresc = -1;
    else if (!strcmp(s->text, "diminuendo)")) in_cresc = 0;
    else if (!strcmp(s->text, ">)")) in_cresc = 0;
}

void AbcSmf::writeSingleNote(int track, struct abc_symbol* s) {
    long delta_tick;

    if (s->text[0] == 'Z' || s->text[0] == 'z') {
        /* no event */
    } else {
        delta_tick = (tpu * s->ev.start_num / s->ev.start_den) - last_tick;
        last_tick += delta_tick;

        /* modify cur_dyn from context */
        setDynamic(delta_tick);

        /* prepend with expression pedal if any */
        writeExpression(track);

        /* set note lyrics if any */
        writeLyric(s->lyr);


        if (s->ev.value) {
            writeMidiEvent(delta_tick, noteon, track, s->ev.key + transpose, cur_dyn * s->ev.value);
        } else {
            long small = tpu * upm / 8;
            small = (delta_tick > small) ? small : delta_tick;
            writeMidiEvent(delta_tick - (small / shorten), noteon, track, s->ev.key + transpose, 0x00); /* note off */
            last_tick -= (small / shorten);
            shorten = in_slur;
        }
    }
}

void AbcSmf::onSMFWriteTempoTrack(void) {
}

void AbcSmf::onSMFWriteTrack(int track) {
    long mspqn = 60000 / tempo;
    writeTempo(0, mspqn);
    writeBpmTempo(0, tempo);
    writeKeySignature(0, mks, mode);

    struct abc_voice* f = abc_pass1_unfold_voice(t->voices[track]);
    struct abc_voice* u = abc_pass2_untie_voice(f, t);
    abc_release_voice(f);
    struct abc_voice* v = abc_pass3_ungroup_voice(u);
    abc_release_voice(u);

    struct abc_symbol* s = v->first;

    last_tick = 0;
    transpose = 0;

    in_cresc = 0;
    mark_dyn = DYN_DEFAULT;
    cur_dyn = mark_dyn; /* current dynamic in the tune */
    grace_tick = 0; /* grace group duration */

    grace_mod = 1.0; /* duration modified for graces */
    expr = EXPRESSION_DEFAULT;
    in_slur = SHORTEN_DEFAULT;
    shorten = in_slur; /* dur will be shortened of 10% of a unit */

    writeMetaEvent(0, 0x03, QString(v->v)); /* textual voice name */

    while (s) {
        switch (s->kind) {
        case ABC_NUP:
        case ABC_CHORD: /* ungrouping is already done */
        case ABC_GRACE:
        case ABC_TIE: /* untying has been already done */
        case ABC_ALT: /* unfolding is already done */
        case ABC_GCHORD:
        case ABC_EOL:
        case ABC_SPACE: {
                break;
        }
        case ABC_CHANGE: {
            if (s->ev.type == EV_KEYSIG) {
                writeKeySignature(0, s->ev.key, s->ev.value);
            } else if (s->ev.type == EV_TEMPO) {
                tempo = s->ev.value;
                long mspqn = 60000 / tempo;
                writeTempo(0, mspqn);
                writeBpmTempo(0, tempo);
            } else if (s->ev.type == EV_METRIC) {
                m = &s->text[2];
                upm = abc_unit_per_measure(l, m);
            } else if (s->ev.type == EV_UNIT) {
                l = &s->text[2];
                /* 'key' = numerator, 'value' = denominator */
                tpu = (s->ev.key * 4 * DPQN) / (s->ev.value);
                upm = abc_unit_per_measure(l, m);
            }
            break;
        }
        case ABC_NOTE: {
            writeSingleNote(track, s);
            break;
        }
        case ABC_DECO: {
            manageDecoration(s);
            break;
        }
        case ABC_SLUR: {
            if (strchr(s->text, '(')) {
                in_slur = 100;
            } else {
                in_slur = SHORTEN_DEFAULT;
            }
            break;
        }
        case ABC_BAR: {
            /* reset measure accidentals */
            break;
        }
        case ABC_INST: {
            int val;
            if (sscanf(s->text, "MIDI program %d", &val)) {
                writeMidiEvent(0, program, track, val);
            } else if (sscanf(s->text, "MIDI transpose %d", &val)) {
                transpose = val;
            }
            break;
        }
        } /* EO case */
        s = s->next;
    }
    writeMetaEvent(0, 0x2F);
    abc_release_voice(v);
}

/* text must be %d/%d */
void AbcSmf::getNumDen(const char* text, long* num, long* den) {
    bool ok;
    QString str(text);
    QStringList sl = str.split('/');
    if (sl.count() < 2) *num = 1, *den = 8;
    *num = sl.at(0).toLong(&ok);
    if (!ok) *num = 1;
    *den = sl.at(1).toLong(&ok);
    if (!ok) *den = 8;
}

int AbcSmf::getSMFKeySignature(const char* text, int* mode) {
    QString str(text);

    *mode = 0;
    if (str.contains("min"))
        *mode = 1;

    if (str.isEmpty() || str == "C" || str == "Cmaj" || str == "Amin")
        return 0;

    if (str == "G" || str == "Gmaj" || str == "Emin")
        return 1;

    if (str == "D" || str == "Dmaj" || str == "Bmin")
        return 2;

    if (str == "A" || str == "Amaj" || str == "F#min")
        return 3;

    if (str == "E" || str == "Emaj" || str == "C#min")
        return 4;

    if (str == "B" || str == "Bmaj" || str == "G#min")
        return 5;

    if (str == "F#" || str == "F#maj" || str == "D#min")
        return 6;

    if (str == "C#" || str == "C#maj" || str == "A#min")
        return 7;


    if (str == "F" || str == "Fmaj" || str == "Dmin")
        return -1;

    if (str == "Bb" || str == "Bbmaj" || str == "Gmin")
        return -2;

    if (str == "Eb" || str == "Ebmaj" || str == "Cmin")
        return -3;

    if (str == "Ab" || str == "Abmaj" || str == "Fmin")
        return -4;

    if (str == "Db" || str == "Dbmaj" || str == "Bbmin")
        return -5;

    if (str == "Gb" || str == "Gbmaj" || str == "Ebmin")
        return -6;

    if (str == "Cb" || str == "Cbmaj" || str == "Abmin")
        return -7;

    return 0;
}

void AbcSmf::setDynamic(long dur) {
    /* dynamics */
    long d = (dur > 4 * tpu ? 4 : dur > 2 * tpu ? 2 : 1);
    if (in_cresc > 0)
        cur_dyn = (cur_dyn + d) < 128 ? cur_dyn + d : 127;
    else if (in_cresc < 0)
        cur_dyn = (cur_dyn - d) > 30 ? cur_dyn - d : 30;
    else
        cur_dyn = mark_dyn;
}

void AbcSmf::writeExpression(int track) {
    if (expr) {
        writeMidiEvent(0, control, track, 0x0b, expr);
        expr = EXPRESSION_DEFAULT;
    }
}

void AbcSmf::writeLyric(const char* l) {
    if (l) {
        writeMetaEvent(0, 0x05, QString(l));
    }
}
