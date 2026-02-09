#include "AbcApplication.h"
#include <fluidsynth.h>
#include <QDebug>
#include <dlfcn.h>
#include "config.h"
#include "settings.h"
#ifdef USE_LIBABCM2PS
#include "../abcm2ps/abcm2ps.h"
#endif
#include "settings.h"

AbcApplication::AbcApplication(int& argc, char** argv)
    : QApplication(argc, argv),
      logwindow(nullptr)
{
    setOrganizationName(SETTINGS_DOMAIN);
	//setOrganizationDomain("herewe");
    setApplicationName(SETTINGS_APP);
    setApplicationVersion(VERSION " (" REVISION ")");

    Settings settings;
    settings.check();

#ifdef USE_LIBABCM2PS
    abcminit();
#endif

    /* check pipewire */
    pipewire_handle = dlopen("libpipewire-0.3.so", RTLD_LAZY);

    /* pipewire init */
    if (pipewire_handle) {
        void* sym = dlsym(pipewire_handle, "pw_init");
        if (!sym) {
            dlclose(pipewire_handle);
            pipewire_handle = NULL;
        } else {
            typedef void (*pw_init_t)(int* argc, char** argv);
            pw_init_t pw_init = (pw_init_t) sym;
            pw_init(&argc, argv);
        }

        fluid_settings_t* fluid_settings = new_fluid_settings();
        if (fluid_settings) {
            if (fluid_settings_setstr(fluid_settings, "audio.driver", "pipewire") == FLUID_OK) {
                fluid_synth_t* fluid_synth = new_fluid_synth(fluid_settings);
                if (fluid_synth) {
                    fluid_audio_driver_t* fluid_adriver = new_fluid_audio_driver(fluid_settings, fluid_synth);
                    if (fluid_adriver) {
                        fluid_has_pipewire = true;
                        delete_fluid_audio_driver(fluid_adriver);
                    }
                    delete_fluid_synth(fluid_synth);
                }
            }
            delete_fluid_settings(fluid_settings);
        }
    }

    logwindow = new LogWindow;
}

AbcApplication::~AbcApplication()
{
    /* pipewire deinit */
    if (pipewire_handle) {
        void* sym = dlsym(pipewire_handle, "pw_deinit");
        if (!sym) {
            dlclose(pipewire_handle);
            pipewire_handle = NULL;
        } else {
            typedef void (*pw_deinit_t)(void);
            pw_deinit_t pw_deinit = (pw_deinit_t) sym;
            pw_deinit();
        }
    }

    delete logwindow;
}

void AbcApplication::setMainWindow(AbcMainWindow* w)
{
	abcmainwindow = w;
}

AbcMainWindow* AbcApplication::mainWindow()
{
	return abcmainwindow;
}

void AbcApplication::openFileNames(const QStringList& fileNames)
{
    ScoreMenu* menu = mainWindow()->scoreMenu();
    for (int i = 0; i < fileNames.length(); i++) {
        QString fileName = fileNames[i];
        QFileInfo info(fileName);
        menu->loadFile(info.absoluteFilePath());
    }
}

bool AbcApplication::isquit = false;

void AbcApplication::quit()
{
    AbcApplication::isquit = true;
    QApplication::quit();
}

LogWindow* AbcApplication::logWindow() const
{
    return logwindow;
}

void AbcApplication::setLogWindow(LogWindow* newLogwindow)
{
    logwindow = newLogwindow;
}

bool AbcApplication::isQuit()
{
    return AbcApplication::isquit;
}
