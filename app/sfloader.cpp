#include "sfloader.h"


SFLoader::SFLoader(fluid_synth_t* synth, const char* soundfont_path, QObject* parent)
    : QThread(parent),
      fid(FLUID_FAILED)
{
    this->synth = synth;
    this->soundfont_path = soundfont_path;
}

int SFLoader::sfid()
{
    return fid;
}

void SFLoader::run()
{
    if (fluid_is_soundfont(soundfont_path))
        fid = fluid_synth_sfload(synth, soundfont_path, 1);
}
