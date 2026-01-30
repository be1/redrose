#include "navmenu.h"
#include "AbcApplication.h"

NavMenu::NavMenu(QWidget *parent) : QMenu(parent)
{
    setTitle(tr("Navigation"));

    addAction(tr("Previous tune"), QKeySequence(Qt::ALT | Qt::Key_Up), this, &NavMenu::onPrevTuneActionTriggered);
    addAction(tr("Next tune"), QKeySequence(Qt::ALT | Qt::Key_Down), this, &NavMenu::onNextTuneActionTriggered);
    addAction(tr("Play tune"), QKeySequence(Qt::CTRL | Qt::Key_Space), this, &NavMenu::onPlayActionTriggered);
    addAction(tr("View tune"), QKeySequence(Qt::CTRL | Qt::Key_Enter), this, &NavMenu::onViewActionTriggered);
    addAction(tr("Previous page"), QKeySequence(Qt::ALT | Qt::Key_Left), this, &NavMenu::onPrevPageActionTriggered);
    addAction(tr("Next page"), QKeySequence(Qt::ALT | Qt::Key_Right), this, &NavMenu::onNextPageActionTriggered);
    addAction(tr("Rewind to first page"), QKeySequence(Qt::ALT | Qt::Key_Home), this, &NavMenu::onHomeActionTriggered);
    addAction(tr("Froward to lastpage"), QKeySequence(Qt::ALT | Qt::Key_End), this, &NavMenu::onEndActionTriggered);
}

void NavMenu::onPrevPageActionTriggered()
{
    ViewVSplitter* vvs = getViewWidget();
    if (false == vvs->turnPage(-1))
        vvs->turnPage(0);
}

void NavMenu::onNextPageActionTriggered()
{
    ViewVSplitter* vvs = getViewWidget();
    if (false == vvs->turnPage(1))
        vvs->turnPage(0);
}

void NavMenu::onHomeActionTriggered() {
    ViewVSplitter* vvs = getViewWidget();
    vvs->gotoPage(1);
}

void NavMenu::onEndActionTriggered() {
    ViewVSplitter* vvs = getViewWidget();
    vvs->gotoPage(vvs->lastPage());
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

ViewVSplitter *NavMenu::getViewWidget() {
    AbcApplication* a = static_cast<AbcApplication*>(qApp);
    AbcMainWindow* w = a->mainWindow();
    MainHSplitter* mhs = w->mainHSplitter();

    ViewVSplitter* vvs = mhs->viewWidget();
    return vvs;
}

// vim:ts=4:sw=4:et
