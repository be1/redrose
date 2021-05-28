#include "sfloader.h"


SFLoader::SFLoader(fluid_synth_t *synth, const char *soundfont_path, QObject *parent)
    : QThread(parent)
{
    this->synth = synth;
    this->soundfont_path = soundfont_path;
}

void SFLoader::run()
{
    int fid = fluid_synth_sfload(synth, soundfont_path, 0);
    emit sfloadFinished(fid);
}
