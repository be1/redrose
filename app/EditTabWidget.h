#ifndef EDITTABWIDGET_H
#define EDITTABWIDGET_H

#include "EditWidget.h"
#include <QTabWidget>



class EditTabWidget: public QTabWidget
{
	Q_OBJECT

public:
	EditTabWidget(QWidget* parent = nullptr);
    ~EditTabWidget();

    int addTab(EditWidget *swidget);
    void removeTab(int index);
    void removeTabs(void);
    void askRemoveTab(int index);

protected slots:
    void onCurrentTabChanged(int index);
    void onCurrentTextModified(bool modified);
};
#endif
