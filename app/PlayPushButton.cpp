#include "PlayPushButton.h"

PlayPushButton::PlayPushButton(QWidget* parent)
	: QPushButton(parent)
{
    play = true;
    setText(tr("&Play"));
    setIcon(QIcon::fromTheme("media-playback-start"));
}

PlayPushButton::~PlayPushButton()
{
}

void PlayPushButton::flip()
{
    if (play) {
        setText(tr("Sto&p"));
        setIcon(QIcon::fromTheme("media-playback-stop"));
    } else {
        setText(tr("&Play"));
        setIcon(QIcon::fromTheme("media-playback-start"));
    }

    play = !play;
}

bool PlayPushButton::isPlay()
{
    return play;
}
