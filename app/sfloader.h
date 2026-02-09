#ifndef SFLOADER_H
#define SFLOADER_H

#include <fluidsynth.h>
#include <QThread>
#include <QObject>

class SFLoader : public QThread {
    Q_OBJECT
public:
    explicit SFLoader(fluid_synth_t* synth, const char* soundfont_path, QObject* parent);
    int sfid();

protected:
    void run();

private:
    fluid_synth_t* synth;
    const char* soundfont_path;
    int fid;
};

#endif // SFLOADER_H
