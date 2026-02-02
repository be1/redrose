#include "abcmodel.h"
#include <QDebug>
#include <QElapsedTimer>
#include "abc.h"

AbcModel::AbcModel() {}

AbcModel::~AbcModel() {
    if (m_charmap) {
        delete[] m_charmap;
    }

    if (m_voice_events) {
        abc_release_voice(m_voice_events);
    }

    if (m_implementation) {
        abc_release_yy(m_implementation);
    }
}

bool AbcModel::fromAbcBuffer(const QByteArray &ba, bool with_charmap) {
    QElapsedTimer et;
    et.start();
    if (m_implementation)
        abc_release_yy(m_implementation);
    qDebug() << "abc_release_yy" << et.elapsed() << "ms";
    m_tune_no = 0;
    m_voice_no = 0;

    m_buffer = ba;
    et.start();
    m_implementation = abc_parse_buffer(ba.constData(), ba.size());
    qDebug() << "abc_parse_buffer" << et.elapsed() << "ms";

    if (m_implementation->error) {
        qWarning() << __func__ << "Parser Error: will break playback follower";
        return false;
    }

    if (with_charmap) {
        et.start();
        createCharMapping();
        qDebug() << "createCharMapping" << et.elapsed() << "ms";
    } else {
        delete[] m_charmap;
        m_charmap = nullptr;
    }

    /* force tune state initialization */
    et.start();
    selectTuneNo(1);
    qDebug() << "selectTuneNo (make_voice_events)" << et.elapsed() << "ms";
    return true;
}

bool AbcModel::selectTuneNo(int no) {
    if (no < 1) {
        qDebug() << "selecting wrong tune" << no;
        return false;
    }

    abc_tune* tune = tuneOfModel(no);

    if (!tune) {
        return false;
    }

    abc_tune_reset(tune); /* reset tune state to defaults */

    /* generate *border effects* on tune yy->tunes[no -1] state
     * due to *first* voice specific instructions, if any. */
    struct abc_voice* first_voice = abc_make_events_for_voice (tune, 0);
    if (first_voice)
        abc_release_voice(first_voice);

    m_tune_no = no;
    return true;
}

bool AbcModel::selectVoiceNo(int tune_no, int no)
{
    if (tune_no == m_tune_no && no == m_voice_no) {
        return true;
    }

    if (tune_no < 1 || no < 1) {
        qDebug () << "wrong tune or voice" << tune_no << no;
        return false;
    }

    if (!selectTuneNo(tune_no)) {
        return false;
    }

    abc_tune* tune = tuneOfModel(tune_no);
    if (!tune) {
        qDebug () << "cannot find wrong tune" << tune_no;
        return false;
    }

    if (no > tune->count)
        return false;

    if (m_voice_events) {
        abc_release_voice (m_voice_events);
        m_voice_events = nullptr;
    }

    m_voice_events = abc_make_events_for_voice(tune, no -1);
    m_voice_no = no;

    return true;
}

int AbcModel::charIndexFromMidiTick(long tick) const
{
    if (!m_voice_events) {
        //qWarning() << __func__ << "Parser error made follower broken.";
        return -1;
    }

    /* search back from last symbol to get latest match */
    struct abc_symbol* s = m_voice_events->last;
    for (; s; s = s->prev) {
        if (s->kind != ABC_NOTE)
            continue;

        long note_tick = DPQN * 4 * s->ev.start_num / s->ev.start_den;

        /* symbols with negative note_tick are invalid */
        if (note_tick >= 0 && note_tick <= tick) {
            if (m_charmap) {
                //qDebug() << note_tick << tick << s->cidx << m_charmap[s->cidx];
                return m_charmap[s->end_pos];
            }
            else
                return -1; /* tell idx is invalid anyway */
                //return s->cidx;
        }
    }

    return -1;
}

/* this takes the QChar index in the Document */
long AbcModel::midiTickFromCharIndex(int uidx) const
{
    if (!m_voice_events || !m_charmap) {
        return -1;
    }

    int cidx = searchIdx(m_charmap, m_buffer.size(), uidx);

    for (struct abc_symbol* s = m_voice_events->first; s; s = s->next) {
        if (s->start_pos + 1 == cidx) {
            long tick = DPQN * 4 * s->ev.start_num / s->ev.start_den;
            return tick;
        }
    }

    return -1;
}

int AbcModel::midiKeyFromCharIndex(int uidx) const {
    if (!m_voice_events || !m_charmap) {
        return -1;
    }

    int cidx = searchIdx(m_charmap, m_buffer.size(), uidx);

    for (struct abc_symbol* s = m_voice_events->first; s; s = s->next) {
        if (s->start_pos +1 == cidx) {
            return s->ev.key;
        }
    }

    return -1;
}

bool AbcModel::hasError() const {
    if (!m_implementation)
        return true;

    return m_implementation->error;
}

int AbcModel::errorLine() const {
    if (!m_implementation)
        return 0;

    return m_implementation->error_line_nr;
}

int AbcModel::errorChar() const {
    if (!m_implementation)
        return 0;

    if (m_charmap)
        return m_charmap[m_implementation->error_char_index] - m_charmap[m_implementation->error_line_index];

    return m_implementation->error_char_nr;
}

abc *AbcModel::implementation() const {
    return m_implementation;
}

abc_tune *AbcModel::tuneOfModel(int tune) const {
    if (!m_implementation) {
        qDebug() << "no implementation yet.";
        return nullptr;
    }
#if 0
    if (tune > m_implementation->count) {
        return nullptr;
    }

    return m_implementation->tunes[tune -1];
#else
    /* X: field can not follow exact tune order */
    return abc_find_tune(m_implementation, tune);
#endif
}

void AbcModel::createCharMapping() {
    if (m_charmap != nullptr)
        delete[] m_charmap;

    m_charmap = new int[m_buffer.size()];

    /* create byte-to-character mapping for
     * playback follower on the ABC text editor */
    QString str = QString::fromUtf8(m_buffer);

    /* initialize with 1:1 mapping */
    for (int i = 0; i < m_buffer.size(); i++) {
        m_charmap[i] = i;
    }
    /* check we have pure ASCII or not */
    if (str.size() == m_buffer.size())
	    return;
    /* some non-ascii: alter map */
    for (int u = 0; u < str.size(); u++) {
        QByteArray ba = str.mid(0, u).toUtf8();
        m_charmap[ba.size()] = u;
    }
}

int AbcModel::searchIdx(int* ordered, int siz, int needle) const
{
#if 1
    /* dichotomic search */
    int s = 0, e = siz;
    while (s <= e) {
        int m = s + (e - s) / 2;
        if (ordered[m] == needle) {
            /* for double-byte chars, take the first byte (m-1) */
            if (m > 0 && ordered[m-1] == needle)
                 return m-1;
            return m;
	}

        if (ordered[m] < needle)
            s = m + 1;
        else
            e = m - 1;
    }
#else
    for (int i = 0; i < siz; i++) {
        if (ordered[i] == needle)
            return i;
    }
#endif
    return -1;
}

#if 0 //TOO SLOW
int AbcModel::xvFromCharIndex(char XorV, int uidx) const
{
    if (XorV != 'x' && XorV != 'X' && XorV != 'v' && XorV != 'V')
        return -1;

    if (!m_implementation || !m_charmap) {
        return -1;
    }

    int cidx = searchIdx(m_charmap, m_buffer.size(), uidx);

    for (int t = 0; t < m_implementation->count; t++) {
        /* check in comments */
        struct abc_tune* tune = m_implementation->tunes[t];
        for (int c = 0; tune->comments[c]; c++) {
            if (tune->comments[c]->start_pos <= cidx && tune->comments[c]->end_pos >= cidx) {
                bool ok = false;
                if (XorV == 'x' || XorV == 'X') {
                    qDebug() << __func__ << tune->x;
                    return tune->x;
                } else if (XorV == 'v' || XorV == 'V') {
                    // FIXME
                    return 1;
                }
            }
        }
        /* check in notes */
        for (int v = 0; v < m_implementation->tunes[t]->count; v++) {
            struct abc_voice* voice = abc_make_events_for_voice (m_implementation->tunes[t], v);
            for (struct abc_symbol* s = voice->first; s; s = s->next) {
                if (s->start_pos <= cidx && s->end_pos >= cidx) {
                    bool ok = false;
                    if (XorV == 'x' || XorV == 'X') {
                        qDebug() << __func__ << s->voice->tune->x;
                        return s->voice->tune->x;
                    } else if (XorV == 'v' || XorV == 'V') {
                        int v = QString(s->voice->v).toInt(&ok);
                        if (ok)
                            return v;
                        qDebug() << "cannot parse V header";
                        return -1;
                    }
                }
            }
        }
    }

    return -1;
}
#endif

// vim:ts=4:sw=4:et
