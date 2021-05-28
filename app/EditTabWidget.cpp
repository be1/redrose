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
    for (int i = editwidgetlist.length() -1; i >= 0; i-- ) {
        removeTab(i);
    }
}

QList<EditWidget*> *EditTabWidget::editWidgetList()
{
    return &editwidgetlist;
}

EditWidget *EditTabWidget::currentEditWidget()
{
    return currenteditwidget;
}

int EditTabWidget::addTab(EditWidget *swidget)
{
    QFileInfo info(*(swidget->fileName()));
    int ret = QTabWidget::addTab(swidget, info.baseName());
    setCurrentWidget(swidget);
    editwidgetlist.append(swidget);
    currenteditwidget = swidget;
    qDebug() << "addTab: " << (*currenteditwidget->fileName());
    currenteditwidget->editVBoxLayout()->onRunClicked();
    return ret;
}

void EditTabWidget::removeTab(int index)
{
    EditWidget *w = editwidgetlist.at(index);
    w->editVBoxLayout()->finalize();
    editwidgetlist.removeAt(index);
    QTabWidget::removeTab(index);
    delete w;
}

void EditTabWidget::removeTabs()
{
    int len = editWidgetList()->length();
    for (int i = len -1; i >= 0; i-- ) {
        removeTab(i);
    }
}

void EditTabWidget::askRemoveTab(int index)
{
    AbcApplication* a = static_cast<AbcApplication*>(qApp);
    AbcMainWindow* m = a->mainWindow();

    EditWidget *w = editwidgetlist.at(index);
    if (!w->editVBoxLayout()->abcPlainTextEdit()->isSaved() &&
            (QMessageBox::StandardButton::No == QMessageBox::question(m, tr("Really close?"),
                                                                      tr("Current score not saved!\nClose this score anyway?"))))
        return;

    m->mainHSplitter()->viewWidget()->cleanup();
    removeTab(index);
}

void EditTabWidget::onCurrentChanged(int index)
{
    EditWidget* swidget = static_cast<EditWidget*>(currentWidget());
    for (int i = 0; i < editwidgetlist.length(); i++) {
        if (editwidgetlist.at(i) == swidget) {
            currenteditwidget = editwidgetlist.at(i);
            qDebug() << "currentTab: " << index << (*currenteditwidget->fileName()) << i;
            currenteditwidget->editVBoxLayout()->onRunClicked();
            break;
        }
    }
}
