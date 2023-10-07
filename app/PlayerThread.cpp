#include "PlayerThread.h"

PlayerThread::PlayerThread(fluid_synth_t* synth, QObject *parent)
    : QThread(parent),
      fluid_player(nullptr),
      fluid_synth(nullptr),
      m_err(false)
{
    fluid_player = new_fluid_player(synth);
    fluid_synth = synth;
}

PlayerThread::~PlayerThread()
{
    stop();
    delete_fluid_player(fluid_player);
}

void PlayerThread::stop()
{
    if (fluid_player_get_status(fluid_player) == FLUID_PLAYER_PLAYING)
        fluid_player_stop(fluid_player);
}

bool PlayerThread::err()
{
    return m_err;
}

int PlayerThread::addMIDIFile(const QString &filename)
{
    QByteArray ba = filename.toUtf8();
    return fluid_player_add(fluid_player, ba.constData());
}

int PlayerThread::addMIDIBuffer(const QByteArray &buf)
{
    return fluid_player_add_mem(fluid_player, buf.constData(), buf.size());
}

void PlayerThread::run()
{
    m_err = fluid_player_play(fluid_player);
    fluid_player_join(fluid_player);
    fluid_synth_all_sounds_off(fluid_synth, -1);
}
