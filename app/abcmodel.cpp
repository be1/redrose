#include "abcmodel.h"
#include <QDebug>
#include "abc.h"

AbcModel::AbcModel() {}

AbcModel::~AbcModel() {
    if (m_implementation) {
        abc_release_yy (m_implementation);
        m_implementation = nullptr;
    }
}

bool AbcModel::fromAbcBuffer(const QByteArray &ba) {
    if (m_implementation)
        abc_release_yy(m_implementation);

    m_implementation = abc_parse_buffer(ba.constData(), ba.size());
    if (m_implementation->error)
        return false;

    return true;
}

void AbcModel::selectTuneNo(int no) {
    m_tune_no = no;
    m_tune = tuneOfModel(no);
    m_ticks_per_unit = ticksPerUnit(m_tune);
}

void AbcModel::selectVoiceNo(int tune_no, int no)
{
    selectTuneNo(tune_no);

    m_voice_no = no;
    m_voice = voiceOfTune(tune_no, no);
    if (m_voice_events)
        abc_release_voice (m_voice_events);

    m_voice_events = abc_make_events_for_voice(m_tune, no -1);
}

int AbcModel::symbolIndexFromAbcEvent(int num, int den) const
{
    for (struct abc_symbol* s = m_voice_events->first; s; s = s->next) {
        if (s->dur_num == num && s->dur_den == den)
            return s->index;
    }

    return 0;
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

int AbcModel::ticksPerUnit(abc_tune* tu) const {
    struct abc_header* lh = abc_find_header(tu, 'L');
    long int num = 1, den = 8;
    getNumDen(lh ? lh->text : "1/8", &num, &den);
    return DPQN * num * 4 / den;
}

int AbcModel::charIndexFromMidiTick(int tick) const
{
    for (struct abc_symbol* s = m_voice_events->first; s; s = s->next) {
        qreal t = m_ticks_per_unit * (qreal) s->ev.start_num * 4 / (qreal) s->ev.start_den;
        if (qRound(t) == tick) {
            return s->cidx;
        }
    }

    return 0;
}

int AbcModel::charIndexFromAbcEvent(int num, int den) const
{
    for (struct abc_symbol* s = m_voice_events->first; s; s = s->next) {
        if (s->ev.start_num == num && s->ev.start_den == den)
            return s->cidx;
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
    if (tune > m_implementation->count) {
        return nullptr;
    }

    return m_implementation->tunes[tune -1];
}

abc_voice *AbcModel::voiceOfTune(int tune, int voice) const {
    if (tune > m_implementation->count) {
        return nullptr;
    }

    if (voice > m_implementation->tunes[tune -1]->count) {
        return nullptr;
    }

    return m_implementation->tunes[tune -1]->voices[voice -1];
}
