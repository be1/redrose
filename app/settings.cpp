#include "settings.h"
#include "config.h"

Settings::Settings(QObject* parent)
    : QSettings(SETTINGS_DOMAIN, SETTINGS_APP, parent)
{

}

void Settings::check()
{

    QVariant sfont = value(SOUNDFONT_KEY);
    if (!sfont.isValid())
        setValue(SOUNDFONT_KEY, DEB_SF2_GM);

    QVariant driver = value(DRIVER_KEY);
    if (!sfont.isValid())
        setValue(DRIVER_KEY, DRIVER_ALSA);

    QVariant player = value(PLAYER_KEY);
    if (!player.isValid())
        setValue(PLAYER_KEY, ABC2MIDI);

#ifndef USE_LIBABCM2PS
    QVariant compiler = value(COMPILER_KEY);
    if (!compiler.isValid())
        setValue(COMPILER_KEY, ABCM2PS);
#endif
    QVariant pstunes = value(PSTUNES_KEY);
    if (!pstunes.isValid())
        setValue(PSTUNES_KEY, TUNES_SELECTED);

    QVariant highlight = value(EDITOR_HIGHLIGHT);
    if (!highlight.isValid())
        setValue(EDITOR_HIGHLIGHT, true);

    QVariant color = value(EDITOR_BAR_COLOR);
    if (!color.isValid())
        setValue(EDITOR_BAR_COLOR, "red");

    color = value(EDITOR_COMMENT_COLOR);
    if (!color.isValid())
        setValue(EDITOR_COMMENT_COLOR, "gray");

    color = value(EDITOR_DECORATION_COLOR);
    if (!color.isValid())
        setValue(EDITOR_DECORATION_COLOR, "orange");

    color = value(EDITOR_EXTRAINSTR_COLOR);
    if (!color.isValid())
        setValue(EDITOR_EXTRAINSTR_COLOR, "blue");

    color = value(EDITOR_GCHORD_COLOR);
    if (!color.isValid())
        setValue(EDITOR_GCHORD_COLOR, "green");

    color = value(EDITOR_HEADER_COLOR);
    if (!color.isValid())
        setValue(EDITOR_HEADER_COLOR, "darkcyan");

    color = value(EDITOR_LYRIC_COLOR);
    if (!color.isValid())
        setValue(EDITOR_LYRIC_COLOR, "magenta");

    sync();
}

void Settings::reset()
{
    QVariant sfont = value(SOUNDFONT_KEY);
    setValue(SOUNDFONT_KEY, DEB_SF2_GM);

    QVariant driver = value(DRIVER_KEY);
    setValue(DRIVER_KEY, DRIVER_ALSA);

    QVariant player = value(PLAYER_KEY);
    setValue(PLAYER_KEY, ABC2MIDI);

#ifndef USE_LIBABCM2PS
    QVariant compiler = value(COMPILER_KEY);
    setValue(COMPILER_KEY, ABCM2PS);
#endif
    QVariant pstunes = value(PSTUNES_KEY);
    setValue(PSTUNES_KEY, TUNES_SELECTED);

    QVariant highlight = value(EDITOR_HIGHLIGHT);
    setValue(EDITOR_HIGHLIGHT, true);

    QVariant color = value(EDITOR_BAR_COLOR);
    setValue(EDITOR_BAR_COLOR, "red");

    color = value(EDITOR_COMMENT_COLOR);
    setValue(EDITOR_COMMENT_COLOR, "gray");

    color = value(EDITOR_DECORATION_COLOR);
    setValue(EDITOR_DECORATION_COLOR, "orange");

    color = value(EDITOR_EXTRAINSTR_COLOR);
    setValue(EDITOR_EXTRAINSTR_COLOR, "blue");

    color = value(EDITOR_GCHORD_COLOR);
    setValue(EDITOR_GCHORD_COLOR, "green");

    color = value(EDITOR_HEADER_COLOR);
    setValue(EDITOR_HEADER_COLOR, "darkcyan");

    color = value(EDITOR_LYRIC_COLOR);
    setValue(EDITOR_LYRIC_COLOR, "magenta");

    sync();

}
