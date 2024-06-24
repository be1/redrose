#include "abcsynth.h"
#include "config.h"
#include <QTimer>
#include <QMutexLocker>
#include <QDebug>
#include "AbcApplication.h"
#include "settings.h"

#if (FLUIDSYNTH_VERSION_MAJOR < 3)
static int handle_midi_event(void *data, fluid_midi_event_t *event) {
    AbcSynth* self = static_cast<AbcSynth*>(data);
    if (self->m_tick == fluid_player_get_current_tick(self->player())) {
        ;
    } else {
        self->m_tick = fluid_player_get_current_tick(self->player());
        emit self->tickChanged(self->m_tick);
    }

    return fluid_synth_handle_midi_event(self->synth(), event);
}
#else
static int handle_midi_tick(void *data, int tick) {
    AbcSynth* self = static_cast<AbcSynth*>(data);

    if (self->m_tick == tick)
        return FLUID_OK;

    self->m_tick = tick;
    emit self->tickChanged(tick);
    return FLUID_OK;
}
#endif

AbcSynth::AbcSynth(const QString& name, QObject* parent)
    : QObject(parent),
      fluid_settings(nullptr),
      fluid_synth(nullptr),
      fluid_adriver(nullptr),
      fluid_player(nullptr),
      sfloader(nullptr),
      sfid(0),
      id(NULL),
      drv(NULL),
      sf(NULL),
      inited(false),
      m_err(false),
      m_secs(1)
{
    Settings settings;

    QByteArray ba;
    ba = settings.value(DRIVER_KEY).toString().toUtf8();
    drv = (char*) realloc(drv, ba.length() + 1);
    strncpy(drv, ba.constData(), ba.length());
    drv[ba.length()] = '\0';

    //qDebug() << name << drv;
    fluid_settings = new_fluid_settings();
    //qDebug() << fluid_settings_setint(fluid_settings, "synth.reverb.active", TRUE);
    //qDebug() << fluid_settings_setnum(fluid_settings, "synth.reverb.room-size", 1.);
    //qDebug() << fluid_settings_setnum(fluid_settings, "synth.reverb.width", 100.);
    //qDebug() << fluid_settings_setnum(fluid_settings, "synth.reverb.damp", 1.);
    //qDebug() << fluid_settings_setnum(fluid_settings, "synth.reverb.level", 1.);

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
        QMessageBox::warning(a->mainWindow(), tr("Error"), tr("No soundfont to load!\nPlease close tab and check player preferences."));
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

    playback_monitor.setInterval(1000);
    connect(&playback_monitor, &QTimer::timeout, this, &AbcSynth::monitorPlayback);
#if 0
    qreal reverb = settings.value(REVERB_KEY).toDouble();
    if (reverb > 0.) {
        qDebug() << fluid_synth_reverb_on(fluid_synth, -1, TRUE);
        qDebug() << fluid_synth_set_reverb_group_roomsize(fluid_synth, -1, 0.2);
        qDebug() << fluid_synth_set_reverb_group_width(fluid_synth, -1, 0.5);
        qDebug() << fluid_synth_set_reverb_group_damp(fluid_synth, -1, 0.);
        qDebug() << fluid_synth_set_reverb_group_level(fluid_synth, -1, reverb);
    } else {
        fluid_synth_reverb_on(fluid_synth, -1, FALSE);
    }
#endif
}

AbcSynth::~AbcSynth()
{
    abortPlay();
    abortSFLoad();

    delete sfloader;
    delete_fluid_audio_driver(fluid_adriver);
    delete_fluid_player(fluid_player);
    delete_fluid_synth(fluid_synth);
    delete_fluid_settings(fluid_settings);
    free(id);
    free(drv);
    free(sf);
}

void AbcSynth::monitorPlayback()
{
    QTimer* mon = qobject_cast<QTimer*>(sender());
    AbcApplication *a = static_cast<AbcApplication*>(qApp);

    if (fluid_player) {
        m_mutex.lock();
        switch (fluid_player_get_status(fluid_player)) {
        case FLUID_PLAYER_READY:
            m_mutex.unlock();
            a->mainWindow()->statusBar()->showMessage(tr("Ready."));
            break;
        case FLUID_PLAYER_PLAYING:
            m_mutex.unlock();
            a->mainWindow()->statusBar()->showMessage(tr("Playing ") + QString::number(m_secs) + "s.");
            m_secs++;
            break;
#if (FLUIDSYNTH_VERSION_MAJOR >= 3)
        case FLUID_PLAYER_STOPPING:
            m_mutex.unlock();
            a->mainWindow()->statusBar()->showMessage(tr("Stopping."));
            break;
#endif
        case FLUID_PLAYER_DONE:
        default:
            m_mutex.unlock();
            /* cleanup and trigger synthFinished */
            stop();
            mon->stop();
            a->mainWindow()->statusBar()->showMessage(tr("Done: ") + QString::number(m_secs) + "s.");
            emit synthFinished(m_err);
            break;
        }
    } else {
        /* stopped before ending: player has been freed and nulled */
        mon->stop();
        a->mainWindow()->statusBar()->showMessage(tr("Done: ") + QString::number(m_secs) + "s.");
        emit synthFinished(m_err);
    }
}

void AbcSynth::abortPlay()
{
    if (isPlaying()) {
        stop();
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
        stop();
    }

    fluid_player = new_fluid_player(fluid_synth);
#if (FLUIDSYNTH_VERSION_MAJOR < 3)
    fluid_player_set_playback_callback(fluid_player, handle_midi_event, this);
#else
    fluid_player_set_tick_callback(fluid_player, handle_midi_tick, this);
#endif
    qDebug() << "Loading " << midifile;
    if (FLUID_FAILED == fluid_player_add(fluid_player, midifile.toUtf8().constData())) {
        a->mainWindow()->statusBar()->showMessage(tr("Cannot load MIDI file: ") + midifile);
        qWarning() << "Cannot load MIDI file: " << midifile;
    } else {
        qDebug() << "Starting file playback with SoundFont " << sf;
        a->mainWindow()->statusBar()->showMessage(tr("Starting synthesis..."));

        m_err = fluid_player_play(fluid_player);
        playback_monitor.start();
    }
}

void AbcSynth::play(const QByteArray& ba)
{
    AbcApplication *a = static_cast<AbcApplication*>(qApp);

    if (isPlaying()) {
        qDebug() << "Synth is playing. Stopping it.";
        stop();
    }

    fluid_player = new_fluid_player(fluid_synth);
#if (FLUIDSYNTH_VERSION_MAJOR < 3)
    fluid_player_set_playback_callback(fluid_player, handle_midi_event, this);
#else
    fluid_player_set_tick_callback(fluid_player, handle_midi_tick, this);
#endif
    if (FLUID_FAILED == fluid_player_add_mem(fluid_player, ba.constData(), ba.size())) {
        qWarning() << "Cannot load MIDI buffer." ;
    } else {
        qDebug() << "Starting buffer playback with SoundFont " << sf;

        a->mainWindow()->statusBar()->showMessage(tr("Starting synthesis..."));
        m_err = fluid_player_play(fluid_player);
        playback_monitor.start();
    }
}

void AbcSynth::fire(int chan, int pgm, int key, int vel)
{
    //qDebug() << "Firing: " << chan << pgm << key << vel;
    fluid_synth_program_change(fluid_synth, chan, pgm);
    fluid_synth_noteon(fluid_synth, chan, key, vel);
    QTimer::singleShot(500, this, [this, chan, key] () { fluid_synth_noteoff(fluid_synth, chan, key); });
}

void AbcSynth::seek(int tick)
{
    QMutexLocker locker(&m_mutex);

    if (fluid_player) {
        fluid_player_seek(fluid_player, tick);
    }
}

int AbcSynth::getTotalTicks()
{
    QMutexLocker locker(&m_mutex);

    if (fluid_player) {
        return fluid_player_get_total_ticks(fluid_player);
    }

    return 0;
}

void AbcSynth::stop()
{
    QMutexLocker locker(&m_mutex);

    m_secs = 1;
    if (fluid_player) {
        fluid_synth_all_notes_off(fluid_synth, -1);
        fluid_player_stop(fluid_player);
        fluid_player_join(fluid_player);
        delete_fluid_player(fluid_player);
        fluid_player = NULL;
    }
}

bool AbcSynth::isLoading()
{
   return !inited;
}

bool AbcSynth::isPlaying()
{
    QMutexLocker locker(&m_mutex);

    if (fluid_player)
        return fluid_player_get_status(fluid_player) == FLUID_PLAYER_PLAYING;
    else
        return false;
}
