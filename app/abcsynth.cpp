#include "abcsynth.h"
#include "config.h"
#include <QTimer>
#include <QDebug>
#include "AbcApplication.h"
#include "settings.h"

AbcSynth::AbcSynth(const QString& name, QObject* parent)
    : QObject(parent),
      fluid_settings(nullptr),
      fluid_synth(nullptr),
      fluid_adriver(nullptr),
      player_thread(nullptr),
      sfloader(nullptr),
      sfid(0),
      id(NULL),
      drv(NULL),
      sf(NULL),
      inited(false)
{
    Settings settings;

    QByteArray ba;
    ba = settings.value(DRIVER_KEY).toString().toUtf8();
    drv = (char*) realloc(drv, ba.length() + 1);
    strncpy(drv, ba.constData(), ba.length());
    drv[ba.length()] = '\0';

    //qDebug() << name << drv;
    fluid_settings = new_fluid_settings();
    fluid_settings_setstr(fluid_settings, "audio.driver", drv);

    if (settings.value(DRIVER_KEY) == DRIVER_JACK) {
        QString jack_id = "redr-" + name;
        ba = jack_id.toUtf8();
        id = (char*) realloc(id, ba.length() + 1);
        strncpy(id, ba.constData(), ba.length());
        id[ba.length()] = '\0';
        fluid_settings_setstr(fluid_settings, "audio.jack.id", id);
    }

    fluid_synth = new_fluid_synth(fluid_settings);
    fluid_synth_set_gain(fluid_synth, 1.0);

    /* start synthesizer thread */
    fluid_adriver = new_fluid_audio_driver(fluid_settings, fluid_synth);

    /* early soundfont load */
    AbcApplication *a = static_cast<AbcApplication*>(qApp);

    QVariant soundfont = settings.value(SOUNDFONT_KEY);
    curSFont = soundfont.toString();
    QFileInfo info(curSFont);
    if (!info.exists()) {
        a->mainWindow()->statusBar()->showMessage(tr("No soundfont to load! Please check settings."));
        return;
    }

    ba = curSFont.toUtf8();
    sf = (char*) realloc(sf, ba.length() + 1);
    strncpy(sf, ba.constData(), ba.length());
    sf[ba.length()] = '\0';

    sfloader = new SFLoader(fluid_synth, sf, this);
    connect(sfloader, &SFLoader::finished, this, &AbcSynth::onSFontFinished);
    a->mainWindow()->statusBar()->showMessage(tr("Loading sound font: ") + sf);
    sfloader->start();
}

AbcSynth::~AbcSynth()
{
    abortPlay();
    abortSFLoad();

    delete sfloader;
    delete player_thread;
    delete_fluid_audio_driver(fluid_adriver);
    delete_fluid_synth(fluid_synth);
    delete_fluid_settings(fluid_settings);
    free(id);
    free(drv);
    free(sf);
}

void AbcSynth::abortPlay()
{
    if (isPlaying()) {
        disconnect(player_thread, &PlayerThread::finished, this, &AbcSynth::onPlayFinished);
        stop();
        player_thread->wait();
    }
}

void AbcSynth::abortSFLoad()
{
    if (sfloader && sfloader->isRunning()) {
        disconnect(sfloader, &SFLoader::finished, this, &AbcSynth::onSFontFinished);
        sfloader->wait();
    }

    inited = false;
}

void AbcSynth::onSFontFinished() {
    SFLoader* sfl = static_cast<SFLoader*>(sender());
    int fid = sfl->sfid();
    AbcApplication *a = static_cast<AbcApplication*>(qApp);

    if (fid == FLUID_FAILED) {
        a->mainWindow()->statusBar()->showMessage(tr("Cannot load sound font: ") + sf);
        inited = false;
        emit initFinished(true);
    } else {
        a->mainWindow()->statusBar()->showMessage(tr("Sound font loaded."));
        sfid = fid;
        inited = true;
        emit initFinished(false);
    }

    delete sfloader;
    sfloader = nullptr;
}


void AbcSynth::play(const QString& midifile) {
    AbcApplication *a = static_cast<AbcApplication*>(qApp);

    if (isPlaying()) {
        qDebug() << "Synth is playing. Stopping it.";
        disconnect(player_thread, &QThread::finished, this, &AbcSynth::onPlayFinished);
        stop();
        waitPlayer();
        delete player_thread;
        player_thread = nullptr;
    }

    player_thread = new PlayerThread(fluid_synth, this);
    connect(player_thread, &PlayerThread::finished, this, &AbcSynth::onPlayFinished);

    qDebug() << "Loading " << midifile;
    if (FLUID_FAILED == player_thread->addMIDIFile(midifile)) {
        a->mainWindow()->statusBar()->showMessage(tr("Cannot load MIDI file: ") + midifile);
        qWarning() << "Cannot load MIDI file: " << midifile;
    } else {
        qDebug() << "Starting playback with SoundFont " << sf;

        a->mainWindow()->statusBar()->showMessage(tr("Starting synthesis..."));

        player_thread->start();

        if (player_thread->err()) {
            a->mainWindow()->statusBar()->showMessage(tr("Synthesis error."));
        }
    }
}

void AbcSynth::play(const QByteArray& ba)
{
    if (isPlaying()) {
        qDebug() << "Synth is playing. Stopping it.";
        stop();
    }

    player_thread = new PlayerThread(fluid_synth, this);
    connect(player_thread, &PlayerThread::finished, this, &AbcSynth::onPlayFinished);

    if (FLUID_FAILED == player_thread->addMIDIBuffer(ba)) {
        qWarning() << "Cannot load MIDI buffer." ;
    } else {
        player_thread->start();
    }
}

void AbcSynth::fire(int chan, int pgm, int key, int vel)
{
    //qDebug() << "Firing: " << chan << pgm << key << vel;
    fluid_synth_program_change(fluid_synth, chan, pgm);
    fluid_synth_noteon(fluid_synth, chan, key, vel);
    QTimer::singleShot(500, this, [this, chan, key] () { fluid_synth_noteoff(fluid_synth, chan, key); });
}

void AbcSynth::stop()
{
    fluid_synth_all_notes_off(fluid_synth, -1);
    fluid_synth_all_sounds_off(fluid_synth, -1);
    if (player_thread && player_thread->isRunning()) {
        disconnect(player_thread, &QThread::finished, this, &AbcSynth::onPlayFinished);
        player_thread->stop();
        waitPlayer();
        bool err = player_thread->err();
        delete player_thread;
        player_thread = nullptr;
        emit synthFinished(err);
    }
}

bool AbcSynth::isLoading()
{
   return !inited;
}

bool AbcSynth::isPlaying()
{
    if (player_thread)
        return player_thread->isRunning();
    else
        return false;
}

void AbcSynth::waitPlayer()
{
    if (player_thread) {
        player_thread->wait();
    }
}

void AbcSynth::onPlayFinished()
{
    bool err = player_thread->err();
    delete player_thread;
    player_thread = nullptr;
    emit synthFinished(err);
}
