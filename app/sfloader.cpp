#include "sfloader.h"


SFLoader::SFLoader(fluid_synth_t *synth, const char *soundfont_path, QObject *parent)
    : QThread(parent)
{
    this->synth = synth;
    this->soundfont_path = soundfont_path;
}

void SFLoader::run()
{
    int fid = FLUID_FAILED;

    if (fluid_is_soundfont(soundfont_path))
        fid = fluid_synth_sfload(synth, soundfont_path, 1);

    emit sfloadFinished(fid);
}
