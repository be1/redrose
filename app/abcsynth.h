#ifndef ABCSYNTH_H
#define ABCSYNTH_H
#include <fluidsynth.h>
#include <QString>
#include <QTimer>
#include <QObject>
#include "sfloader.h"

class AbcSynth: public QObject
{
    Q_OBJECT
public:
    explicit AbcSynth(const QString& name, QObject* parent = nullptr);
    ~AbcSynth();
    void abortPlay(void);
    void abortSFLoad(void);
    void play(const QString& midifile);
    void play(const QByteArray& ba);
    void fire(int chan, int pgm, int key, int vel);
    void stop(void); /* synchronous */
    bool isLoading(void);
    bool isPlaying(void);

signals:
    void initFinished(bool err);
    void synthFinished(int ret);

private slots:
    void onSFontFinished();
    void monitorPlayback();

private:
    QTimer playback_monitor;
    fluid_settings_t* fluid_settings;
    fluid_synth_t* fluid_synth;
    fluid_audio_driver_t* fluid_adriver;
    fluid_player_t* fluid_player;
    SFLoader *sfloader;
    QString curSFont;
    int sfid;
    char *id; /* jack identifier */
    char *drv; /* "alsa" or "pulseaudi"o or "jack" */
    char *sf; /* soundfont file name */
    bool inited;
    bool m_err;
    int m_secs; /* elapsed seconds */
};

#endif // ABCSYNTH_H
