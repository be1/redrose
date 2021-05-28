#ifndef SCOREMENU_H
#define SCOREMENU_H

#include <QMenu>
#include <QMessageBox>

class ScoreMenu: public QMenu
{
	Q_OBJECT

public:
	ScoreMenu(QWidget* parent = nullptr);
    ~ScoreMenu();
    QMessageBox::StandardButton gracefulQuit();

protected slots:
    void onQuitActionTriggered();
    void onOpenActionTriggered();
    void onSaveActionTriggered();
    void onSaveAsActionTriggered();
    void onExportActionTriggered();
    void onExportPsActionTriggered();
    void onCloseActionTriggered();
    void onNewActionTriggered();

private:
    QAction newaction;
    QAction openaction;
    QAction saveaction;
    QAction saveasaction;
    QAction exportaction;
    QAction exppsaction;
    QAction closeaction;
    QAction quitaction;
};

#define NEW_TEMPLATE "X:1\nT:Melody from «Le lac des cygnes»\nC:Piotr Ilitch Tchaïkovski\nM:C\nL:1/4\nK:Amin\n!mf!ABcd|e3c|e3A|cAFc|A4-|AdcB|e3c|e3A|cAFc|A4|]"
#endif
