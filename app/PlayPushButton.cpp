#include "PlayPushButton.h"

PlayPushButton::PlayPushButton(QWidget* parent)
	: QPushButton(parent)
{
    setFocusPolicy(Qt::NoFocus);
    play = true;
    setText(tr("&Play"));
#if (QT_VERSION < QT_VERSION_CHECK(6, 7, 0))
    setIcon(QIcon::fromTheme("media-playback-start"));
#else
    setIcon(QIcon::fromTheme(QIcon::ThemeIcon::MediaPlaybackStart));
#endif
}

PlayPushButton::~PlayPushButton()
{
}

void PlayPushButton::flip()
{
    if (play) {
        setText(tr("Sto&p"));
#if (QT_VERSION < QT_VERSION_CHECK(6, 7, 0))
        setIcon(QIcon::fromTheme("media-playback-stop"));
#else
        setIcon(QIcon::fromTheme(QIcon::ThemeIcon::MediaPlaybackStop));
#endif
    } else {
        setText(tr("&Play"));
#if (QT_VERSION < QT_VERSION_CHECK(6, 7, 0))
        setIcon(QIcon::fromTheme("media-playback-start"));
#else
        setIcon(QIcon::fromTheme(QIcon::ThemeIcon::MediaPlaybackStart));
#endif
    }

    play = !play;
}

bool PlayPushButton::isPlay()
{
    return play;
}
