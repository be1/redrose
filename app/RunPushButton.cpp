#include "RunPushButton.h"

RunPushButton::RunPushButton(QWidget* parent)
	: QPushButton(parent)
{
    setFocusPolicy(Qt::NoFocus);
    setText(tr("View score"));
    QIcon::setFallbackThemeName("HighContrast");
    setIcon(QIcon::fromTheme(QIcon::ThemeIcon::DocumentPrintPreview));
}

RunPushButton::~RunPushButton()
{
}
