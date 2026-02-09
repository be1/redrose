#include "EditWidget.h"
#include <QTabWidget>

EditWidget::EditWidget(const QString& fileName, QWidget* parent)
    : QWidget(parent),
      editvboxlayout(fileName),
      filename(fileName)
{
	setObjectName("EditWidget:" + fileName);
    setLayout(&editvboxlayout);
}

EditWidget::~EditWidget()
{
}

EditVBoxLayout* EditWidget::editVBoxLayout()
{
	return &editvboxlayout;
}

QString* EditWidget::fileName()
{
	return &filename;
}

void EditWidget::setFileName(const QString& fileName)
{
	filename = fileName;
	editVBoxLayout()->setFileName(fileName);
}
