#ifndef PDFWIDGET_H
#define PDFWIDGET_H

#include <QPlainTextEdit>

class LogView: public QPlainTextEdit
{
	Q_OBJECT

public:
    LogView(QWidget* parent = nullptr);
    ~LogView();

private:
};
#endif
