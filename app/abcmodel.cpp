#include "abcmodel.h"
#include <QDebug>
#include "abc.h"

AbcModel::AbcModel() {}

AbcModel::~AbcModel() {
    if (m_charmap) {
        delete[] m_charmap;
    }

    if (m_voice_events) {
        abc_release_voice (m_voice_events);
    }

    if (m_implementation) {
        abc_release_yy (m_implementation);
    }
}

bool AbcModel::fromAbcBuffer(const QByteArray &ba, bool with_charmap) {
    if (m_implementation)
        abc_release_yy(m_implementation);

    m_buffer = ba;
    m_implementation = abc_parse_buffer(ba.constData(), ba.size());
    if (m_implementation->error) {
        qWarning() << __func__ << "Parser Error: will break playback follower";
        return false;
    }

    if (with_charmap)
        createCharMapping();
    else {
        delete[] m_charmap;
        m_charmap = nullptr;
    }

    return true;
}

bool AbcModel::selectTuneNo(int no) {
    if (no < 1)
        return false;

    abc_tune* tune = tuneOfModel(no);

    if (!tune)
        return false;

    m_tune_no = no;
    return true;
}

bool AbcModel::selectVoiceNo(int tune_no, int no)
{
    if (tune_no < 1 || no < 1)
        return false;

    if (!selectTuneNo(tune_no))
        return false;

    abc_tune* tune = tuneOfModel(tune_no);
    if (!tune)
        return false;

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
        return 0;
    }

    const long whole_in_ticks = DPQN * 4;

    /* search back from last symbol to avoid finding matching past event */
    struct abc_symbol* s = m_voice_events->last;
    while (s) {
        long t = whole_in_ticks * ((qreal) s->ev.start_num / (qreal) s->ev.start_den);

        /* symbols with t == 0 are not notes (num/den == 0/1) */
        if (t && t <= tick) {
            if (m_charmap)
                return m_charmap[s->cidx];
            else
                return 0; /* tell idx is invalid anyway */
                //return s->cidx;
        }

        s = s->prev;
    }

    return 0;
}

/*this takes the QChar index in the Document */
long AbcModel::midiTickFromCharIndex(int uidx) const
{
    if (!m_voice_events || !m_charmap)
        return 0;

    int cidx = searchIdx(m_charmap, m_buffer.size(), uidx);

    for (struct abc_symbol* s = m_voice_events->first; s; s = s->next) {
        if (s->cidx >= cidx) {
            long tick = DPQN * 4 * s->ev.start_num / s->ev.start_den;
            return tick;
        }
    }

    return 0;
}

bool AbcModel::hasError() const {
    return m_implementation->error;
}

int AbcModel::errorLine() const {
    return m_implementation->error_line;
}

int AbcModel::errorChar() const {
    return m_implementation->error_char;
}

abc *AbcModel::implementation() const {
    return m_implementation;
}

abc_tune *AbcModel::tuneOfModel(int tune) const {
    if (!m_implementation)
        return nullptr;

    if (tune > m_implementation->count) {
        return nullptr;
    }

    return m_implementation->tunes[tune -1];
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
        if (ordered[m] == needle)
            return m;

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
