#ifndef HELPMENU_H
#define HELPMENU_H

#include <QMenu>

class HelpMenu: public QMenu
{
	Q_OBJECT

public:
	HelpMenu(QWidget* parent = nullptr);
	~HelpMenu();

protected slots:
void onAboutActionTriggered();
void onAboutQtActionTriggered();

private:
    QAction aboutaction;
    QAction aboutqtaction;
};
#endif
