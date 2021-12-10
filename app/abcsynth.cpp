#include "abcsynth.h"
#include "config.h"
#include <QDebug>
#include "AbcApplication.h"
#include "settings.h"

AbcSynth::AbcSynth(const QString& name, QObject* parent)
    : QObject(parent),
      fluid_settings(nullptr),
      fluid_synth(nullptr),
      fluid_player(nullptr),
      fluid_adriver(nullptr),
      waiter(nullptr),
      sfloader(nullptr),
      sfid(0),
      id(NULL),
      drv(NULL),
      sf(NULL),
      mf(NULL),
      inited(false)
{
    Settings settings;
    QByteArray ba;
    ba = settings.value(DRIVER_KEY).toString().toUtf8();
    drv = (char*) realloc(drv, ba.length() + 1);
    strncpy(drv, ba.constData(), ba.length());
    drv[ba.length()] = '\0';

    qDebug() << name << drv;
    fluid_settings = new_fluid_settings();
    fluid_settings_setstr(fluid_settings, "audio.driver", drv);

    QString jack_id = "qabc-" + name;
    ba = jack_id.toUtf8();
    id = (char*) realloc(id, ba.length() + 1);
    strncpy(id, ba.constData(), ba.length());
    id[ba.length()] = '\0';
    fluid_settings_setstr(fluid_settings, "audio.jack.id", id);

    fluid_synth = new_fluid_synth(fluid_settings);
    fluid_synth_set_gain(fluid_synth, 1.0);
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
    connect(sfloader, &SFLoader::sfloadFinished, this, &AbcSynth::onSFontFinished);
    a->mainWindow()->statusBar()->showMessage(tr("Loading sound font: ") + sf);
    sfloader->start();
}

AbcSynth::~AbcSynth()
{
    /* stop synthesis */
    if (fluid_player && waiter && waiter->isRunning()) {
        fluid_player_stop(fluid_player);
        waiter->wait();
        delete_fluid_player(fluid_player);
    }

    delete_fluid_audio_driver(fluid_adriver);
    delete_fluid_synth(fluid_synth);
    delete_fluid_settings(fluid_settings);
    free(id);
    free(drv);
    free(sf);
    free(mf);
}

void AbcSynth::abort()
{
   if (sfloader && sfloader->isRunning())
       sfloader->terminate();
   if (sfloader)
       sfloader->deleteLater();
   sfloader = nullptr;
}

void AbcSynth::onSFontFinished(int fid) {
    inited = true;
    AbcApplication *a = static_cast<AbcApplication*>(qApp);
    if (fid == FLUID_FAILED) {
        a->mainWindow()->statusBar()->showMessage(tr("Cannot load sound font: ") + sf);
        emit initFinished(true);
    } else {
        a->mainWindow()->statusBar()->showMessage(tr("Sound font loaded."));
        sfid = fid;
        emit initFinished(false);
    }
}


void AbcSynth::play(const QString& midifile) {
    AbcApplication *a = static_cast<AbcApplication*>(qApp);

    if (waiter && waiter->isRunning()) {
        fluid_player_stop(fluid_player);
        waiter->wait();
        waiter->deleteLater();
    }

     delete_fluid_player(fluid_player);

    fluid_player = new_fluid_player(fluid_synth);
    waiter = new TuneWaiter(fluid_player, this);
    connect(waiter, &TuneWaiter::playerFinished, this, &AbcSynth::onPlayFinished);

    /* midi file can change from tune (xspinbox) index */
    QByteArray ba;
    ba = midifile.toUtf8();
    mf = (char*) realloc(mf, ba.length() + 1);
    strncpy(mf, ba.constData(), ba.length());
    mf[ba.length()] = '\0';

    qDebug() << mf;

    if (FLUID_FAILED == fluid_player_add(fluid_player, mf)) {
        a->mainWindow()->statusBar()->showMessage(tr("Cannot load MIDI file: ") + mf);
    }

    fluid_player_play(fluid_player);
    waiter->start();

    switch (fluid_player_get_status(fluid_player)) {
    case FLUID_PLAYER_READY:
        a->mainWindow()->statusBar()->showMessage(tr("Starting synthesis..."));
        break;
    case FLUID_PLAYER_PLAYING:
        a->mainWindow()->statusBar()->showMessage(tr("Synthesis playing..."));
        break;
    case FLUID_PLAYER_DONE:
        a->mainWindow()->statusBar()->showMessage(tr("Synthesis done."));
        break;
    default:
        break;
    }
}

void AbcSynth::stop()
{
   if (waiter && waiter->isRunning())
       fluid_player_stop(fluid_player);
}

bool AbcSynth::isLoading()
{
   return !inited;
}

bool AbcSynth::isPlaying()
{
    if (waiter)
            return waiter->isRunning();
    else
        return false;
}

void AbcSynth::waitFinish()
{
    if (waiter)
            waiter->wait();
}

void AbcSynth::onPlayFinished(int ret)
{
    qDebug() << ret;

    AbcApplication *a = static_cast<AbcApplication*>(qApp);
    a->mainWindow()->statusBar()->showMessage(tr("Synthesis done."));
    emit synthFinished(ret);
}
