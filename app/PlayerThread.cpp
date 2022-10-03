#include "PlayerThread.h"

PlayerThread::PlayerThread(fluid_player_t* p, QObject *parent)
    : QThread(parent)
    , player(p)
{

}

void PlayerThread::abort()
{
    if (player)
        fluid_player_stop(player);
}

void PlayerThread::run()
{
    int ret = 0;
    if (player)
        ret = fluid_player_join(player);

    delete_fluid_player(player);
    player = nullptr;

    emit playerFinished(ret);
}
