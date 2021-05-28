#ifndef PLAYPUSHBUTTON_H
#define PLAYPUSHBUTTON_H

#include <QPushButton>

class PlayPushButton: public QPushButton
{
	Q_OBJECT

public:
	PlayPushButton(QWidget* parent = nullptr);
    ~PlayPushButton();
    void flip();
    bool isPlay();

private:
    bool play;
};
#endif
