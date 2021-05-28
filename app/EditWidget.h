#ifndef EDITWIDGET_H
#define EDITWIDGET_H

#include "EditVBoxLayout.h"
#include <QWidget>
#include <QTabWidget>

class EditWidget: public QWidget
{
	Q_OBJECT

public:
    explicit EditWidget(const QString& fileName, QWidget* parent = nullptr);
    ~EditWidget();

    EditVBoxLayout *editVBoxLayout();
    QString *fileName();
    void setFileName(const QString& fileName);

private:
	EditVBoxLayout editvboxlayout;
    QString filename;
};

#endif
