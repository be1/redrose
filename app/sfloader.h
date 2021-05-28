#ifndef SFLOADER_H
#define SFLOADER_H

#include <fluidsynth.h>
#include <QThread>
#include <QObject>

class SFLoader : public QThread {
    Q_OBJECT
public:
    explicit SFLoader(fluid_synth_t *synth, const char* soundfont_path, QObject *parent);

signals:
    void sfloadFinished(int sfid);

protected:
    void run();

private:
    fluid_synth_t* synth;
    const char* soundfont_path;
};

#endif // SFLOADER_H
