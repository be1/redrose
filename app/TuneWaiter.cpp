#include "TuneWaiter.h"

TuneWaiter::TuneWaiter(fluid_player_t* p, QObject *parent)
    : QThread(parent)
    , player(p)
{

}

void TuneWaiter::run()
{
    int ret = 0;
    if (player)
        ret = fluid_player_join(player);

    delete_fluid_player(player);
    player = nullptr;

    emit playerFinished(ret);
}
