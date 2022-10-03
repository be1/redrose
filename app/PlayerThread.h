#ifndef PLAYERTHREAD_H
#define PLAYERTHREAD_H

#include <fluidsynth.h>
#include <QThread>
#include <QObject>

class PlayerThread : public QThread
{
    Q_OBJECT
public:
    /**
     * @brief PlayerThread
     * @param p fluid_player, PlayerThread takes ownership.
     * @param parent
     */
    explicit PlayerThread(fluid_player_t *p, QObject *parent = nullptr);
    void abort();

signals:
    void playerFinished(int code);

protected:
    void run() override;

private:
    fluid_player_t *player = nullptr;
};

#endif // PLAYERTHREAD_H
