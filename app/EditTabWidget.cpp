#include "AbcApplication.h"
#include "EditTabWidget.h"
#include <QFileInfo>
#include <QDebug>
#include <QMessageBox>

EditTabWidget::EditTabWidget(QWidget* parent)
	: QTabWidget(parent)
{
    setTabsClosable(true);
    connect(this, &QTabWidget::tabCloseRequested, this, &EditTabWidget::askRemoveTab);
    connect(this, &QTabWidget::currentChanged, this, &EditTabWidget::onCurrentChanged);
}

EditTabWidget::~EditTabWidget()
{
    for (int i = count() -1; i >= 0; i-- ) {
        removeTab(i);
    }
}

int EditTabWidget::addTab(EditWidget *swidget)
{
    QFileInfo info(*(swidget->fileName()));
    int ret = QTabWidget::addTab(swidget, info.baseName());
    setCurrentWidget(swidget);
    qDebug() << "addTab: " << (*swidget->fileName());
    return ret;
}

void EditTabWidget::removeTab(int index)
{
    EditWidget *w = qobject_cast<EditWidget*>(widget(index));
    w->editVBoxLayout()->finalize();
    QTabWidget::removeTab(index);
    delete w;
}

void EditTabWidget::removeTabs()
{
    for (int i = count() -1; i >= 0; i-- ) {
        removeTab(i);
    }
}

void EditTabWidget::askRemoveTab(int index)
{
    AbcApplication* a = static_cast<AbcApplication*>(qApp);
    AbcMainWindow* m = a->mainWindow();

    EditWidget *w = qobject_cast<EditWidget*>(widget(index));
    if (!w->editVBoxLayout()->abcPlainTextEdit()->isSaved() &&
            (QMessageBox::StandardButton::No == QMessageBox::question(m, tr("Really close?"),
                                                                      tr("Current score not saved!\nClose this score anyway?"))))
        return;

    m->mainHSplitter()->viewWidget()->cleanup();
    removeTab(index);
}

void EditTabWidget::onCurrentChanged(int index)
{
    if (index < 0)
        return;

    EditWidget* w = qobject_cast<EditWidget*>(widget(index));
    qDebug() << "currentTab: " << index << (*w->fileName()) << index;
    w->editVBoxLayout()->onRunClicked();
}
