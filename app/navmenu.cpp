#include "navmenu.h"
#include "AbcApplication.h"

NavMenu::NavMenu(QWidget *parent) : QMenu(parent)
{
    setTitle(tr("Navigation"));

    addAction(tr("Previous tune"), QKeySequence(Qt::ALT | Qt::Key_Left), this, &NavMenu::onPrevTuneActionTriggered);
    addAction(tr("Next tune"), QKeySequence(Qt::ALT | Qt::Key_Right), this, &NavMenu::onNextTuneActionTriggered);
    addAction(tr("Play tune"), QKeySequence(Qt::ALT | Qt::Key_P), this, &NavMenu::onPlayActionTriggered);
    addAction(tr("View tune"), QKeySequence(Qt::ALT | Qt::Key_V), this, &NavMenu::onViewActionTriggered);
}

void NavMenu::onPrevTuneActionTriggered()
{
    EditVBoxLayout* evl = getCurrentLayout();
    if (!evl)
        return;

    int cur = evl->xSpinBox()->value();
    if (cur > 1)
        evl->xSpinBox()->setValue(cur -1);
}

void NavMenu::onNextTuneActionTriggered()
{
    EditVBoxLayout* evl = getCurrentLayout();
    if (!evl)
        return;

    int cur = evl->xSpinBox()->value();
    evl->xSpinBox()->setValue(cur +1);
}

void NavMenu::onPlayActionTriggered()
{
    EditVBoxLayout* evl = getCurrentLayout();
    if (!evl)
        return;

    evl->playPushButton()->click();
}

void NavMenu::onViewActionTriggered()
{
    EditVBoxLayout* evl = getCurrentLayout();
    if (!evl)
        return;

    evl->runPushButton()->click();
}

EditVBoxLayout *NavMenu::getCurrentLayout()
{
    AbcApplication* a = static_cast<AbcApplication*>(qApp);
    AbcMainWindow* w = a->mainWindow();
    MainHSplitter* mhs = w->mainHSplitter();
    EditTabWidget* etw = mhs->editTabWidget();
    EditWidget* ew = static_cast<EditWidget*> (etw->currentWidget());
    if (!ew)
        return nullptr;

    EditVBoxLayout* evl = ew->editVBoxLayout();
    return evl;
}
