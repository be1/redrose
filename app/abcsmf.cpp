#include <QDebug>
#include "abcsmf.h"

AbcSmf::AbcSmf(struct abc* yy, int vel, int shorter, bool expr, int x, QObject *parent) : QObject(parent),
        m_yy(yy),
        m_x(x),
        m_tune(nullptr),
        m_keysig(nullptr),
        m_unit_length(nullptr),
        m_metric(nullptr),
        m_tick_per_unit(0),
        m_unit_per_whole(0),
        m_tempo(0),
        m_emphasis(0),
        m_last_tick(0),
        m_in_slur(shorter),
        m_in_cresc(0),
        m_mark_dyn(vel),
        m_cur_dyn(vel),
        m_grace_tick(0),
        m_transpose(0),
        m_default_velocity(vel),
        m_default_shorter(shorter),
        m_manage_expression(expr),
        m_smf(nullptr)
{
    m_tune = abc_find_tune(yy, x);
    if (!m_tune)
        return;

    m_smf = smf_new();
    if (smf_set_ppqn(m_smf, DPQN)) {
        smf_delete(m_smf);
        return;
    }

    writeAll();
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

    m_unit_per_whole = abc_unit_per_measure(m_unit_length, "4/4");
    struct abc_header* qh = abc_find_header(m_tune, 'Q');
    if (qh)
        m_tempo = abc_tempo(qh->text);
    else
        m_tempo = 120;

    long num = 1, den = 8;
    getNumDen(m_unit_length, &num, &den);

    m_tick_per_unit = DPQN * num * 4 / den;

    struct abc_header* kh = abc_find_header(m_tune, 'K');
    if (!kh) {
        m_keysig = nullptr, m_midi_mode = 0;
    } else {
        m_keysig = kh->text;
        m_midi_keysig = getSMFKeySignature(m_keysig, &m_midi_mode);
    }

    /* reset dynamics */
    m_cur_dyn = m_mark_dyn = m_default_velocity;
    m_cur_expression = 0; /* will change on track start */
}

bool AbcSmf::isDynamicEnd(const struct abc_symbol* deco) {
    if (deco->kind == ABC_DECO) {
        return (!strcmp(deco->text, "crescendo)") || !strcmp(deco->text, "<)") || !strcmp(deco->text, "diminuendo)") || !strcmp(deco->text, ">)"));
    }

    return false;
}

void AbcSmf::applyDecoration(struct abc_symbol* s) {
    if (isDynamicMark(s)) {
        m_mark_dyn = m_cur_dyn = getDynamic(s->text, m_minimum_velocity, m_cur_dyn);
    }

    if (!strcmp(s->text, "sfz")) m_mark_dyn = m_cur_dyn = 100;
    else if (!strcmp(s->text, ".")) m_shorter = 50;
    else if (!strcmp(s->text, "H")) m_shorter = 100;
    else if (!strcmp(s->text, "tenuto")) m_shorter = 100;
    else if (!strcmp(s->text, "L")) m_emphasis = 5;
    else if (!strcmp(s->text, ">")) m_emphasis = 5;
    else if (!strcmp(s->text, "accent")) m_emphasis = 5;
    else if (!strcmp(s->text, "emphasis")) m_emphasis = 5;
    else if (!strcmp(s->text, "crescendo(") || !strcmp(s->text, "<(")) {
        m_in_cresc = 1;
        m_first_note_in_cresc = 1;
        m_cresc_duration = 0;
    }
    else if (!strcmp(s->text, "crescendo)") || !strcmp(s->text, "<)")) {
        m_in_cresc = m_cresc_duration = 0;
    }
    else if (!strcmp(s->text, "diminuendo(") || !strcmp(s->text, ">(")) {
        m_in_cresc = -1;
        m_first_note_in_cresc = 1;
        m_cresc_duration = 0;
    }
    else if (!strcmp(s->text, "diminuendo)") ||!strcmp(s->text, ">)")) {
        m_in_cresc = m_cresc_duration = 0;
    }

    if (m_first_note_in_cresc) {
        struct abc_symbol* last_note_in_cresc = findClosingDynamics(s, &m_cresc_duration);
        m_climax_velocity = getDynAfter(last_note_in_cresc, m_minimum_velocity, m_cur_dyn);
        unsigned char expression_climax = getDynAfter(last_note_in_cresc, m_minimum_expression, m_cur_expression);

        long last_cut = computeCut(last_note_in_cresc);
        m_cresc_duration -= last_cut;
        /* spread at most 24 expression events over (de)crescendo. */
        generateExpressionArray(expression_climax);
        m_first_note_in_cresc = 0;
    }
}

bool AbcSmf::isDynamicMark(const struct abc_symbol* deco) {
    if (deco->kind == ABC_DECO) {
        return (!strncmp(deco->text, "p", 1) || !strncmp(deco->text, "f", 1) || !strncmp(deco->text, "mp", 2) || !strncmp(deco->text, "mf", 2));
    }

    return false;
}

unsigned char AbcSmf::getDynAfter(struct abc_symbol* deco, unsigned char base, unsigned char dflt) {
    while (deco) {
        if (isDynamicEnd(deco)) {
            struct abc_symbol* s = deco->next;
            while (s) {
                if(isDynamicMark(s))
                    return getDynamic(s->text, base, dflt);

                if (s->kind == ABC_NOTE)
                    break;

                s = s->next;
            }

            /* no mark found */
            if (m_in_cresc > 0) {
                return dflt + 8 > 127 ? dflt : dflt + 16;
            } else if (m_in_cresc < 0) {
                return dflt - 8 < base ? dflt : dflt - 16;
            } else {
                return m_mark_dyn;
            }
        }

        deco = deco->next;
    }

    return m_mark_dyn;
}

struct abc_symbol* AbcSmf::findClosingDynamics(struct abc_symbol* s, long* duration) {
    /* input s must be the first note in (de)cresc */
    struct abc_symbol *last_in_cresc = nullptr;
    long cresc_duration = 0;
    struct abc_symbol* same_chord = nullptr;

    /* until we find a (de)crescendo end */
    while (s && s->kind != ABC_DECO || (strcmp(s->text, "crescendo)") && strcmp(s->text, "<)") && strcmp(s->text, "diminuendo)") && strcmp(s->text, ">)"))) {
        /* use noteOn only */
        if (s->kind == ABC_NOTE && s->ev.value) {

            /* some chords have short and long (or tied) notes.
             * we look for the first (leftmost) note of the chord.
             * if no chord, we take the single note duration too.
             */
            if (!s->of_chord || s->of_chord != same_chord) {
                cresc_duration += m_tick_per_unit * m_unit_per_whole * (qreal) s->dur_num / (qreal) s->dur_den;
                same_chord = s->of_chord;
            }

            last_in_cresc = s;
        }

        s = s->next;
    }

    if (duration)
        *duration = cresc_duration;

    return last_in_cresc;
}

int AbcSmf::writeExpression (smf_track_t* track, long delta, unsigned char chan, unsigned char value) {
    if (value != m_cur_expression) {
        writeMidiEvent(track, delta, (unsigned char) (0xb0 | chan), 11, value);

        m_cur_expression = value;
        return 1;
    }

    return 0;
}

long AbcSmf::computeCut(struct abc_symbol* s) {
    /* this works both for noteOn and noteOff since libabc duplicates the values */

    if (!s)
        return 0;

    /* use note duration stored in note to compute shortening */
    qreal ticks_per_whole = m_tick_per_unit * m_unit_per_whole;
    qreal smaller = ticks_per_whole * ((qreal) (s->dur_num * m_shorter) / 100) / (qreal) s->dur_den;
    qreal cut = ticks_per_whole * (qreal) s->dur_num / (qreal) s->dur_den - smaller;
    return cut;
}

void AbcSmf::generateExpressionArray(unsigned char climax) {
    long cc_delta_tick = (qreal) m_cresc_duration / m_expression_ticks;
    char diff = qAbs(climax - m_cur_expression);
    qreal interval = (qreal) diff / (qreal) m_expression_ticks;

    for (long i = 0; i < m_expression_ticks; i++) {
        unsigned char expression = m_cur_expression + (m_in_cresc * i) * interval;

        /* (should not happen) */
        if (expression >= 127 || expression <= m_minimum_expression) /* "pppp" */
            break;

        m_expression_array.append(QPair<long, unsigned char>(cc_delta_tick, expression));
    }
}

void AbcSmf::writeSingleNoteEvent(smf_track_t* track, unsigned char chan, struct abc_symbol* s) {
    /* s->kind must be of ABC_NOTE. We won't check it... */

    if (s->text[0] == 'Z' || s->text[0] == 'z' || s->text[0] == 'X' || s->text[0] == 'x') {
        /* silence means no event */
    } else {
        /* can be noteOn or noteOff */

        const qreal whole_in_ticks = m_tick_per_unit * m_unit_per_whole;
        long next_note_tick = whole_in_ticks * ((qreal) s->ev.start_num / (qreal) s->ev.start_den);
        long next_note_delta_tick = next_note_tick - m_last_tick;

        /* set note lyrics if any */
        //writeLyric(track, s->lyr);

        if (s->ev.value) {
            /* this 's' noteOn duration is */
            const long note_duration = whole_in_ticks * (qreal) s->dur_num / (qreal) s->dur_den;

            /* noteOn event */
            unsigned char vel = m_cur_dyn + m_emphasis > 127 ? 127 : m_cur_dyn + m_emphasis;
            vel = vel < m_minimum_velocity ? m_minimum_velocity : vel;

            writeMidiEvent(track, next_note_delta_tick, m_noteon | chan, s->ev.key + m_transpose, vel * s->ev.value);

            /* modify next cur_dyn *after*: this allow start of (de)cresc to be smooth (even if note_duration isn't next one) */
            m_cur_dyn = computeNextVelocity(note_duration, m_cur_dyn);

            /* update last tick */
            m_last_tick = next_note_tick;
        } else {
            /* noteOff cut (see user prefs) */
            long cut = computeCut(s);

            /* write a series of expression events until the current note tick */
            while (!m_expression_array.isEmpty()) {
                QPair<long, unsigned char> ev = m_expression_array.takeFirst();

                if (ev.first >= next_note_delta_tick - cut) {
                    m_expression_array.prepend(ev);
                    break;
                }

                if (m_manage_expression) {
                    if (writeExpression(track, ev.first, chan, ev.second)) {
                        next_note_delta_tick -= ev.first;
                    }
                }
            }

            /* now write actual noteOff (with cut) */
            writeMidiEvent(track, next_note_delta_tick - cut, m_noteon | chan, s->ev.key + m_transpose, 0x00); /* note off */

            /* reset duration and length after singlenote decorations */
            m_shorter = m_in_slur;
            m_emphasis = 0;

            /* update last tick */
            m_last_tick = next_note_tick - cut;
        }
    }
}

void AbcSmf::writeAll(){
    for (int i = 0; i < m_tune->count; i++) {
        smf_track_t* track = smf_track_new();
        smf_add_track(m_smf, track);
        writeTrack(track, i);
    }
}

void AbcSmf::writeTrack(smf_track_t* track, int voice_nr) {
    char chan = voice_nr;

    reset();

    if (!m_tune)
        return;

    int sluring = 0;
    writeBpmTempo(track, m_tempo);
    writeKeySignature(track, m_midi_keysig, m_midi_mode);

    /* transform voice nr 'track' from tune 'm_tune' into MIDI-aware voice 'v' */
    struct abc_voice* v = abc_make_events_for_voice(m_tune, voice_nr);

    struct abc_symbol* s = v->first;

    m_last_tick = 0;
    m_transpose = 0;

    m_in_cresc = 0;
    m_mark_dyn = m_default_velocity;
    m_cur_dyn = m_mark_dyn; /* current dynamic in the tune */
    m_grace_tick = 0; /* grace group duration */

    m_grace_mod = 1.0; /* duration modified for graces */

    /* by default, we are not in slurs */
    m_in_slur = m_default_shorter;
    m_shorter = m_in_slur;

    writeName(track, v->v); /* textual voice name */

    if (m_manage_expression)
        writeExpression (track, 0, chan, m_default_expression);

    /* process list of symbols */
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
                writeKeySignature(track, s->ev.key, s->ev.value);
            } else if (s->ev.type == EV_TEMPO) {
                m_tempo = s->ev.value;
                //long mspqn = 60000 / tempo;
                //writeTempo(0, mspqn);
                writeBpmTempo(track, m_tempo);
            } else if (s->ev.type == EV_METRIC) {
                m_metric = &s->text[2];
            } else if (s->ev.type == EV_UNIT) {
                m_unit_length = &s->text[2];
                /* 'key' = numerator, 'value' = denominator */
                m_unit_per_whole = abc_unit_per_measure(m_unit_length, "4/4");
                m_tick_per_unit = (DPQN * 4 * s->ev.key) / (s->ev.value);
            }
            break;
        }
        case ABC_NOTE: {
            writeSingleNoteEvent(track, chan, s);
            break;
        }
        case ABC_DECO: {
            applyDecoration(s);
            break;
        }
        case ABC_SLUR: {
            if (strchr(s->text, '(')) {
                sluring++;
                m_shorter = m_in_slur = 100;
            } else {
                if (sluring < 2)
                    m_shorter = m_in_slur = m_default_shorter;

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
                writeMidiEvent(track, 0, m_program | chan, val);
            } else if (sscanf(s->text, "MIDI transpose %d", &val)) {
                m_transpose = val;
            } else if (sscanf(s->text, "MIDI channel %d", &val)) {
                chan = val -1;
                if (m_manage_expression)
                    writeExpression (track, 0, chan, m_default_expression);
            }
            break;
        }
        } /* switch */
        s = s->next;
    }
    if (smf_track_add_eot_delta_pulses(track, 0)) {
        abc_release_voice(v);
        return;
    }

    abc_release_voice(v);
}

void AbcSmf::saveToFile(const char *filename) {
    if (smf_save(m_smf, filename))
        ;
}

AbcSmf::~AbcSmf() {
    for (int i = m_smf->number_of_tracks; i > 0; i--) {
        smf_track_t* track = smf_get_track_by_number(m_smf, i);

        for (int j = track->number_of_events; j > 0; j--) {
            smf_event_t* event = smf_track_get_event_by_number(track, j);

            smf_event_remove_from_track(event);
            smf_event_delete(event);
        }

        smf_track_remove_from_smf(track);
        smf_track_delete(track);
    }
}

char AbcSmf::getDynamic(const char* text, unsigned char base, unsigned char dflt)
{
    if (!strcmp(text, "pppp")) return base; /* 40 */
    else if (!strcmp(text, "ppp")) return base + 16;
    else if (!strcmp(text, "pp")) return base + 24;
    else if (!strcmp(text, "p")) return base + 32;
    else if (!strcmp(text, "mp")) return base + 40;
    else if (!strcmp(text, "mf")) return base + 48;
    else if (!strcmp(text, "f")) return base + 56;
    else if (!strcmp(text, "ff")) return base + 64;
    else if (!strcmp(text, "fff")) return base + 72;
    else if (!strcmp(text, "ffff")) return base + 80;
    else if (!strcmp(text, "sfz")) return base + 80; /* 120 */
    return dflt;
}

/* text must be %d/%d */
void AbcSmf::getNumDen(const char* text, long* num, long* den) {
    bool ok;
    QString str(text);
    QStringList sl = str.split('/');
    if (sl.size() < 2) {
        *num = 1;
        *den = 8;
        return;
    }

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

unsigned char AbcSmf::computeNextVelocity(long dur, unsigned char curvel) {
    /* the more long the note duration, the more delta_dyn */
    long delta_dyn = m_climax_velocity * (qreal) dur / (qreal) m_cresc_duration;

    if (m_in_cresc > 0)
        /* in crescendo */
        return (curvel + delta_dyn) < m_climax_velocity ? curvel + delta_dyn : m_climax_velocity;
    else if (m_in_cresc < 0)
        /* in diminuendo */
        return (curvel - delta_dyn) > m_climax_velocity ? curvel - delta_dyn : m_climax_velocity;
    else
        /* a dynamic has been explicitely marked */
        return m_mark_dyn;
}

void AbcSmf::writeName(smf_track_t* track, const char* l) {
    if (l) {
        smf_event_t* event = smf_event_new_textual(0x03, l);
        smf_track_add_event_delta_pulses(track, event, 0);
    }
}

void AbcSmf::writeLyric(smf_track_t* track, const char* l) {
    if (l) {
        smf_event_t* event = smf_event_new_textual(0x05, l);
        smf_track_add_event_delta_pulses(track, event, 0);
    }
}

void AbcSmf::writeBpmTempo(smf_track_t *track, long val) {
    unsigned int ms = 60000000;
    unsigned int set = ms / val;
    unsigned char set1 = (set & 0xFF0000) >> 16, set2 = (set & 0x00FF00) >> 8, set3 = set & 0x0000FF;
    unsigned char data[6] = { 0xFF, 0x51, 0x03, set1, set2, set3 };
    smf_event_t* event = smf_event_new_from_pointer(data, 6);
    smf_track_add_event_delta_pulses(track, event, 0);
}

void AbcSmf::writeTimeSignature(smf_track_t *track, unsigned char numerator, unsigned char denominator) {
    unsigned char pow = 0;
    while (denominator >= 0) {
        denominator <<= 1;
        pow++;
    }

    unsigned char data[7] = { 0xFF, 0x58, 0x04, numerator, pow, 24, 8 };
    smf_event_t* event = smf_event_new_from_pointer(data, 7);
    smf_track_add_event_delta_pulses(track, event, 0);
}

void AbcSmf::writeKeySignature(smf_track_t *track, unsigned char keysig, unsigned char mode) {
    unsigned char data[5] = { 0xFF, 0x59, 0x02, keysig, mode };
    smf_event_t* event = smf_event_new_from_pointer(data, 5);
    smf_track_add_event_delta_pulses(track, event, 0);
}

void AbcSmf::writeMidiEvent(smf_track_t *track, long delta, unsigned char ev_type, unsigned char ev_key, unsigned char ev_val) {
    smf_event_t* event = smf_event_new_from_bytes(ev_type, ev_key, ev_val);
    smf_track_add_event_delta_pulses(track, event, delta);
}

void AbcSmf::writeMidiEvent(smf_track_t *track, long delta, unsigned char ev_type, unsigned char ev_val) {
    smf_event_t* event = smf_event_new_from_bytes(ev_type, ev_val, -1);
    smf_track_add_event_delta_pulses(track, event, delta);
}

void AbcSmf::writeMidiEvent(smf_track_t *track, long delta, unsigned char ev_type) {
    smf_event_t* event = smf_event_new_from_bytes(ev_type, -1, -1);
    smf_track_add_event_delta_pulses(track, event, delta);
}

void AbcSmf::writeMidiEvent(smf_track_t *track, long delta, unsigned char ev_type, const char *ev_val) {
    smf_event_t* event = smf_event_new_textual(ev_type, ev_val);
    smf_track_add_event_delta_pulses(track, event, delta);
}
