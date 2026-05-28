#include "RunPushButton.h"

RunPushButton::RunPushButton(QWidget* parent)
	: QPushButton(parent)
{
    setFocusPolicy(Qt::NoFocus);
    setText(tr("View score"));
    //setIcon(QIcon::fromTheme(QIcon::ThemeIcon::DocumentPrintPreview));
    setIcon(QIcon::fromTheme("document-print-preview"));
}

RunPushButton::~RunPushButton()
{
}
