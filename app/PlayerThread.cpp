#include "PlayerThread.h"

PlayerThread::PlayerThread(fluid_synth_t* synth, QObject *parent)
    : QThread(parent)
{
    fluid_player = new_fluid_player(synth);
}

PlayerThread::~PlayerThread()
{
    abort();
}

void PlayerThread::abort()
{
    if (fluid_player)
        fluid_player_stop(fluid_player);
}

int PlayerThread::status()
{
    if (fluid_player)
        return fluid_player_get_status(fluid_player);
    return 0;
}

int PlayerThread::addMIDIFile(const QString &filename)
{
    QByteArray ba = filename.toUtf8();
    char f[ba.size() + 1];
    memcpy(f, ba.constData(), ba.size());
    f[ba.size()] = '\0';
    return fluid_player_add(fluid_player, f);
}

int PlayerThread::addMIDIBuffer(const QByteArray &buf)
{
    return fluid_player_add_mem(fluid_player, buf.constData(), buf.size());
}

void PlayerThread::run()
{
    int ret = 0;
    if (fluid_player) {
        fluid_player_play(fluid_player);
        ret = fluid_player_join(fluid_player);
        delete_fluid_player(fluid_player);
        fluid_player = nullptr;
    }

    emit playerFinished(ret);
}
