#include <QDebug>
#include <QTextCodec>
#include <drumstick/qsmf.h>
#include "abcsmf.h"

AbcSmf::AbcSmf(struct abc* yy, int vel, int short_den, int x, QObject *parent) : drumstick::File::QSmf(parent),
        m_yy(yy),
        m_x(x),
        m_tune(nullptr),
        m_keysig(nullptr),
        m_unit_length(nullptr),
        m_metric(nullptr),
        m_tick_per_unit(0),
        m_unit_per_measure(0),
        m_tempo(0),
        m_emphasis(0),
        m_last_tick(0),
        m_note_dur(0),
        m_in_slur(short_den),
        m_in_cresc(0),
        m_mark_dyn(vel),
        m_cur_dyn(vel),
        m_grace_tick(0),
        m_noteon(0x90),
        m_program(0xc0),
        m_control(0xb0),
        m_transpose(0),
        m_default_velocity(vel),
        m_default_shorten(short_den)
{
    connect(this, &QSmf::signalSMFWriteTempoTrack, this, &AbcSmf::onSMFWriteTempoTrack);
    connect(this, &QSmf::signalSMFWriteTrack, this, &AbcSmf::onSMFWriteTrack);

    m_tune = abc_find_tune(yy, x);
    if (!m_tune)
        return;

    setDivision(DPQN);
    //setTextCodec(QTextCodec::codecForName("UTF-8"));
    setFileFormat(1);
    setTracks(m_tune->count);
}

void AbcSmf::reset() {
    m_tune = abc_find_tune(m_yy, m_x);
    if (!m_tune)
        return;

    struct abc_header* lh = abc_find_header(m_tune, 'L');
    if (lh)
        m_unit_length = lh->text;
    else
        m_unit_length = "1/8";
    struct abc_header* mh = abc_find_header(m_tune, 'M');
    if (mh)
        m_metric = mh->text;
    else
        m_metric = "4/4";

    m_unit_per_measure = abc_unit_per_measure(m_unit_length, m_metric);
    struct abc_header* qh = abc_find_header(m_tune, 'Q');
    if (qh)
        m_tempo = abc_tempo(qh->text);
    else
        m_tempo = 120;

    long num = 1, den = 8;
    getNumDen(m_unit_length, &num, &den);

    m_tick_per_unit = (num * 4 * DPQN) / (den);

    struct abc_header* kh = abc_find_header(m_tune, 'K');
    if (!kh) {
        m_keysig = nullptr, m_midi_mode = 0;
    } else {
        m_keysig = kh->text;
        m_midi_keysig = getSMFKeySignature(m_keysig, &m_midi_mode);
    }
}

void AbcSmf::manageDecoration(struct abc_symbol* s) {
    if (!strcmp(s->text, "pppp")) m_mark_dyn = m_cur_dyn = 8;
    else if (!strcmp(s->text, "ppp")) m_mark_dyn = m_cur_dyn = 16;
    else if (!strcmp(s->text, "pp")) m_mark_dyn = m_cur_dyn = 32;
    else if (!strcmp(s->text, "p")) m_mark_dyn = m_cur_dyn = 48;
    else if (!strcmp(s->text, "mp")) m_mark_dyn = m_cur_dyn = 64;
    else if (!strcmp(s->text, "mf")) m_mark_dyn = m_cur_dyn = 80;
    else if (!strcmp(s->text, "f")) m_mark_dyn = m_cur_dyn = 96;
    else if (!strcmp(s->text, "ff")) m_mark_dyn = m_cur_dyn = 112;
    else if (!strcmp(s->text, "fff")) m_mark_dyn = m_cur_dyn = 124;
    else if (!strcmp(s->text, "ffff")) m_mark_dyn = m_cur_dyn = 127;
    else if (!strcmp(s->text, "sfz")) m_mark_dyn = m_cur_dyn = 100;
    else if (!strcmp(s->text, ".")) m_shorten = 2;
    else if (!strcmp(s->text, "H")) m_shorten = 100;
    else if (!strcmp(s->text, "tenuto")) m_shorten = 100;
    else if (!strcmp(s->text, "L")) m_emphasis = 24;
    else if (!strcmp(s->text, ">")) m_emphasis = 24;
    else if (!strcmp(s->text, "accent")) m_emphasis = 24;
    else if (!strcmp(s->text, "emphasis")) m_emphasis = 24;
    else if (!strcmp(s->text, "crescendo(")) m_in_cresc = 1;
    else if (!strcmp(s->text, "<(")) m_in_cresc = 1;
    else if (!strcmp(s->text, "crescendo)")) m_in_cresc = 0;
    else if (!strcmp(s->text, "<)")) m_in_cresc = 0;
    else if (!strcmp(s->text, "diminuendo(")) m_in_cresc = -1;
    else if (!strcmp(s->text, ">(")) m_in_cresc = -1;
    else if (!strcmp(s->text, "diminuendo)")) m_in_cresc = 0;
    else if (!strcmp(s->text, ">)")) m_in_cresc = 0;
}

void AbcSmf::writeSingleNote(int chan, struct abc_symbol* s) {
    long delta_tick;

    if (s->text[0] == 'Z' || s->text[0] == 'z' || s->text[0] == 'X' || s->text[0] == 'x') {
        /* no event */
    } else {
        delta_tick = (m_tick_per_unit * s->ev.start_num / s->ev.start_den) - m_last_tick;
        m_last_tick += delta_tick;

        /* modify cur_dyn from context */
        setDynamic(delta_tick);

        /* set note lyrics if any */
        writeLyric(s->lyr);

        if (s->ev.value) {
            writeMidiEvent(delta_tick, m_noteon, chan, s->ev.key + m_transpose, (m_cur_dyn + m_emphasis) * s->ev.value);
        } else {
            long small = m_tick_per_unit * m_unit_per_measure / 8;
            small = (delta_tick > small) ? small : delta_tick;
            writeMidiEvent(delta_tick - (small / m_shorten), m_noteon, chan, s->ev.key + m_transpose, 0x00); /* note off */
            m_last_tick -= (small / m_shorten);
            /* reset after singlenote decorations */
            m_shorten = m_in_slur;
            m_emphasis = 0; /* reset to normal dynamic */
        }
    }
}

void AbcSmf::onSMFWriteTempoTrack(void) {
    qDebug() << "tempo track?";
}

void AbcSmf::onSMFWriteTrack(int track) {
    reset();

    if (!m_tune)
        return;

    int sluring = 0;
    int chan = track;
    //long mspqn = 60000 / tempo;
    //writeTempo(0, mspqn);
    writeBpmTempo(0, m_tempo);
    writeKeySignature(0, m_midi_keysig, m_midi_mode);

    /* transform voice nr 'track' from tune 't' into MIDI-aware voice 'v' */
    struct abc_voice* v = abc_make_events_for_voice(m_tune, track);

    struct abc_symbol* s = v->first;

    m_last_tick = 0;
    m_transpose = 0;

    m_in_cresc = 0;
    m_mark_dyn = m_default_velocity;
    m_cur_dyn = m_mark_dyn; /* current dynamic in the tune */
    m_grace_tick = 0; /* grace group duration */

    m_grace_mod = 1.0; /* duration modified for graces */
    m_in_slur = m_default_shorten;
    m_shorten = m_in_slur; /* dur will be shortened of 1/10 of a unit */

    writeMetaEvent(0, 0x03, QString(v->v)); /* textual voice name */

    while (s) {
        switch (s->kind) {
        case ABC_NUP:
        case ABC_CHORD: /* ungrouping is already done */
        case ABC_GRACE:
        case ABC_TIE:   /* untying is already done */
        case ABC_ALT:   /* unfolding is already done */
        case ABC_GCHORD:
        case ABC_EOL:
        case ABC_SPACE: {
                break;
        }
        case ABC_CHANGE: {
            if (s->ev.type == EV_KEYSIG) {
                writeKeySignature(0, s->ev.key, s->ev.value);
            } else if (s->ev.type == EV_TEMPO) {
                m_tempo = s->ev.value;
                //long mspqn = 60000 / tempo;
                //writeTempo(0, mspqn);
                writeBpmTempo(0, m_tempo);
            } else if (s->ev.type == EV_METRIC) {
                m_metric = &s->text[2];
                m_unit_per_measure = abc_unit_per_measure(m_unit_length, m_metric);
            } else if (s->ev.type == EV_UNIT) {
                m_unit_length = &s->text[2];
                /* 'key' = numerator, 'value' = denominator */
                m_tick_per_unit = (s->ev.key * 4 * DPQN) / (s->ev.value);
                m_unit_per_measure = abc_unit_per_measure(m_unit_length, m_metric);
            }
            break;
        }
        case ABC_NOTE: {
            writeSingleNote(chan, s);
            break;
        }
        case ABC_DECO: {
            manageDecoration(s);
            break;
        }
        case ABC_SLUR: {
            if (strchr(s->text, '(')) {
                sluring++;
                m_shorten = m_in_slur = 100;
            } else {
                if (sluring < 2)
                    m_shorten =  m_in_slur = m_default_shorten;

                if (sluring > 0)
                    sluring--;
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
                writeMidiEvent(0, m_program, chan, val);
            } else if (sscanf(s->text, "MIDI transpose %d", &val)) {
                m_transpose = val;
            } else if (sscanf(s->text, "MIDI channel %d", &val)) {
                chan = val -1;
            }
            break;
        }
        } /* switch */
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
    long d = (dur > 4 * m_tick_per_unit ? 4 : dur > 2 * m_tick_per_unit ? 2 : 1);
    if (m_in_cresc > 0)
        m_cur_dyn = (m_cur_dyn + d) < 128 ? m_cur_dyn + d : 127;
    else if (m_in_cresc < 0)
        m_cur_dyn = (m_cur_dyn - d) > 30 ? m_cur_dyn - d : 30;
    else
        m_cur_dyn = m_mark_dyn;
}

void AbcSmf::writeLyric(const char* l) {
    if (l) {
        writeMetaEvent(0, 0x05, QString(l));
    }
}
