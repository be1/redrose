#include "RunPushButton.h"

RunPushButton::RunPushButton(QWidget* parent)
	: QPushButton(parent)
{
    setFocusPolicy(Qt::NoFocus);
    setText(tr("View score"));
#if (QT_VERSION < QT_VERSION_CHECK(6, 7, 0))
    setIcon(QIcon::fromTheme("document-print-preview"));
#else
    setIcon(QIcon::fromTheme(QIcon::ThemeIcon::DocumentPrintPreview));
#endif
}

RunPushButton::~RunPushButton()
{
}
