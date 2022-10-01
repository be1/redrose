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

signals:
    void playerFinished(int code);

protected:
    void run() override;

private:
    fluid_player_t *player = nullptr;
};

#endif // TUNEWAITER_H
