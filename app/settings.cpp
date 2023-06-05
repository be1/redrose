#include "settings.h"
#include "config.h"

Settings::Settings(QObject* parent)
    : QSettings(SETTINGS_DOMAIN, SETTINGS_APP, parent)
{

}

void Settings::check()
{
    QVariant velocity = value(PLAYER_VELOCITY);
    if (!velocity.isValid())
        setValue(PLAYER_VELOCITY, 80);

    QVariant duration = value(PLAYER_DURATION);
    if (!duration.isValid() || duration.toInt() <= 0)
        setValue(PLAYER_DURATION, 10);

    QVariant autoplay = value(EDITOR_AUTOPLAY);
    if (!autoplay.isValid())
        setValue(EDITOR_AUTOPLAY, false);

    QVariant sfont = value(SOUNDFONT_KEY);
    if (!sfont.isValid())
        setValue(SOUNDFONT_KEY, DEB_SF2_GM);

    QVariant driver = value(DRIVER_KEY);
    if (!sfont.isValid())
        setValue(DRIVER_KEY, DRIVER_ALSA);

    QVariant player = value(PLAYER_KEY);
    if (!player.isValid())
        setValue(PLAYER_KEY, LIBABC2SMF);

#ifndef USE_LIBABCM2PS
    QVariant compiler = value(COMPILER_KEY);
    if (!compiler.isValid())
        setValue(COMPILER_KEY, ABCM2PS);
#endif
    QVariant pstunes = value(PSTUNES_KEY);
    if (!pstunes.isValid())
        setValue(PSTUNES_KEY, TUNES_SELECTED);

    QVariant font_range = value(EDITOR_FONT_RANGE);
    if (!font_range.isValid())
        setValue(EDITOR_FONT_RANGE, 0);

    QVariant highlight = value(EDITOR_HIGHLIGHT);
    if (!highlight.isValid())
        setValue(EDITOR_HIGHLIGHT, false);

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
    setValue(PLAYER_VELOCITY, 80);

    setValue(PLAYER_DURATION, 10);

    setValue(EDITOR_AUTOPLAY, false);

    setValue(SOUNDFONT_KEY, DEB_SF2_GM);

    setValue(DRIVER_KEY, DRIVER_ALSA);

    setValue(PLAYER_KEY, LIBABC2SMF);

#ifndef USE_LIBABCM2PS
    setValue(COMPILER_KEY, ABCM2PS);
#endif
    setValue(PSTUNES_KEY, TUNES_SELECTED);

    setValue(EDITOR_FONT_RANGE, 0);

    setValue(EDITOR_HIGHLIGHT, false);

    setValue(EDITOR_BAR_COLOR, "red");

    setValue(EDITOR_COMMENT_COLOR, "gray");

    setValue(EDITOR_DECORATION_COLOR, "orange");

    setValue(EDITOR_EXTRAINSTR_COLOR, "blue");

    setValue(EDITOR_GCHORD_COLOR, "green");

    setValue(EDITOR_HEADER_COLOR, "darkcyan");

    setValue(EDITOR_LYRIC_COLOR, "magenta");

    sync();
}
