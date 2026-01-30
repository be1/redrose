#ifndef NAVMENU_H
#define NAVMENU_H

#include "EditVBoxLayout.h"
#include "ViewVSplitter.h"
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
    void onPrevPageActionTriggered();
    void onNextPageActionTriggered();
    void onHomeActionTriggered();
    void onEndActionTriggered();

private:
    EditVBoxLayout* getCurrentLayout();
    ViewVSplitter* getViewWidget();
};

#endif // NAVMENU_H
