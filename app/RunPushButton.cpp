#include "RunPushButton.h"

RunPushButton::RunPushButton(QWidget* parent)
	: QPushButton(parent)
{
    setText(tr("&View score"));
    setIcon(QIcon::fromTheme("document-print-preview"));
}

RunPushButton::~RunPushButton()
{
}
