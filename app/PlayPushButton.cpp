#include "PlayPushButton.h"

PlayPushButton::PlayPushButton(QWidget* parent)
	: QPushButton(parent)
{
    setFocusPolicy(Qt::NoFocus);
    play = true;
    setText(tr("&Play"));
    QIcon::setFallbackThemeName("HighContrast");
    setIcon(QIcon::fromTheme(QIcon::ThemeIcon::MediaPlaybackStart));
}

PlayPushButton::~PlayPushButton()
{
}

void PlayPushButton::flip()
{
    if (play) {
        setText(tr("Sto&p"));
        setIcon(QIcon::fromTheme(QIcon::ThemeIcon::MediaPlaybackStop));
    } else {
        setText(tr("&Play"));
        setIcon(QIcon::fromTheme(QIcon::ThemeIcon::MediaPlaybackStart));
    }

    play = !play;
}

bool PlayPushButton::isPlay()
{
    return play;
}
