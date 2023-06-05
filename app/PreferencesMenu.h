#ifndef PREFERENCESMENU_H
#define PREFERENCESMENU_H

#include <QMenu>

class PreferencesMenu: public QMenu
{
	Q_OBJECT

public:
	PreferencesMenu(QWidget* parent = nullptr);
	~PreferencesMenu();

protected slots:
    void onPlayerActionTriggered();
    void onPsActionTriggered();
    void onResetActionTriggered();
    void onEditorActionTriggered();
};
#endif
