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

    m_ticks_per_unit = ticksPerUnit(tune);
    m_units_per_whole = unitsPerWhole(tune);
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

/* text must be %d/%d */
void AbcModel::getNumDen(const char* text, long* num, long* den) const {
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

long AbcModel::ticksPerUnit(abc_tune* tu) const {
    struct abc_header* lh = abc_find_header(tu, 'L');
    long int num = 1, den = 8;
    getNumDen(lh ? lh->text : "1/8", &num, &den);
    return DPQN * num * 4 / den;
}

long AbcModel::unitsPerWhole(abc_tune *tu) const {
    struct abc_header* lh = abc_find_header(tu, 'L');
    return abc_unit_per_measure(lh ? lh->text : "1/8", "4/4");
}

int AbcModel::charIndexFromMidiTick(int tick) const
{
    if (!m_voice_events) {
        //qWarning() << __func__ << "Parser error made follower broken.";
        return 0;
    }

    const long whole_in_ticks = m_ticks_per_unit * m_units_per_whole;
    struct abc_symbol* s = m_voice_events->last;

    /* search backward */
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
    QString str = QString::fromUtf8(m_buffer);

    /* a brute force way to create byte-to-character mapping
     * for playback follower on the ABC text editor */
    for (int n = 0; n < m_buffer.size(); n++) {
        int uni_index = n > str.size() ? str.size() : n;
        QByteArray ba;
        do {
            QString sub = str.mid(0, uni_index);
            ba = sub.toUtf8();

            if (ba.size() == n)
                break;

            uni_index--;
        } while (ba.size() > n);

        m_charmap[n] = uni_index;
    }
}
