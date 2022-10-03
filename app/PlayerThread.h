#ifndef PLAYERTHREAD_H
#define PLAYERTHREAD_H

#include <fluidsynth.h>
#include <QThread>
#include <QObject>

class PlayerThread : public QThread
{
    Q_OBJECT
public:
    explicit PlayerThread(fluid_synth_t* synth, QObject *parent = nullptr);
    void abort();
    int status();
    int addMIDIFile(const QString& filename);
    int addMIDIBuffer(const QByteArray& buf);

signals:
    void playerFinished(int code);

protected:
    void run() override;

private:
    fluid_player_t *fluid_player = nullptr;
};

#endif // PLAYERTHREAD_H
