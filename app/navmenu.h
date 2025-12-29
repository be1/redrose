#ifndef NAVMENU_H
#define NAVMENU_H

#include "EditVBoxLayout.h"
#include <QMenu>

class NavMenu : public QMenu
{
    Q_OBJECT

public:
    NavMenu(QWidget* parent = nullptr);

protected slots:
    void onPrevTuneActionTriggered();
    void onNextTuneActionTriggered();
    void onPlayActionTriggered();
    void onViewActionTriggered();

private:
    EditVBoxLayout* getCurrentLayout();
};

#endif // NAVMENU_H
