#ifndef TUNEWAITER_H
#define TUNEWAITER_H

#include <fluidsynth.h>
#include <QThread>
#include <QObject>

class TuneWaiter : public QThread
{
    Q_OBJECT
public:
    explicit TuneWaiter(fluid_player_t *p, QObject *parent = nullptr);
    void run() override;

signals:
    void playerFinished(int code);

private:
    fluid_player_t *player = nullptr;
};

#endif // TUNEWAITER_H
