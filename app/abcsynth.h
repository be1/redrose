#ifndef ABCSYNTH_H
#define ABCSYNTH_H
#include <fluidsynth.h>
#include <QString>
#include <QObject>
#include "sfloader.h"
#include "TuneWaiter.h"

class AbcSynth: public QObject
{
    Q_OBJECT
public:
    explicit AbcSynth(const QString& name, QObject* parent = nullptr);
    ~AbcSynth();
    void abort(void);
    void play(const QString& midifile);
    void stop(void);
    bool isLoading(void);
    bool isPlaying(void);
    void waitFinish(void);

signals:
    void initFinished(bool err);
    void synthFinished(bool err);

private slots:
    void onSFontFinished(int fid);
    void onPlayFinished(int ret);

private:
    fluid_settings_t* fluid_settings;
    fluid_synth_t* fluid_synth;
    fluid_player_t* fluid_player;
    fluid_audio_driver_t* fluid_adriver;
    TuneWaiter *waiter;
    SFLoader *sfloader;
    QString curSFont;
    int sfid;
    char *id; /* jack identifier */
    char *drv; /* "alsa" or "pulseaudi"o or "jack" */
    char *sf; /* soundfont file name */
    char *mf; /* current midi file name */
    bool inited;
};

#endif // ABCSYNTH_H
