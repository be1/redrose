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
    bool loadFile(const QString& fileName);

protected:
    void setRecentFile(const QString& fileName, bool ok);
    void updateRecentFileActions();
    QString strippedName(const QString &fullFileName);

protected slots:
    void onQuitActionTriggered();
    void onOpenActionTriggered();
    void onOpenRecentActionTriggered();
    void onSaveActionTriggered();
    void onSaveAsActionTriggered();
    void onExportActionTriggered();
    void onExportPsActionTriggered();
    void onCloseActionTriggered();
    void onNewActionTriggered();

private:
    QAction newaction;
    QAction openaction;

    enum { MaxRecentFiles = 5 };
    QAction *recentFileActs[MaxRecentFiles];

    static const QRegularExpression m_abcext;
};

#define NEW_TEMPLATE "X:1\nT:Melody from «Le lac des cygnes»\nC:Piotr Ilitch Tchaïkovski\nM:C\nL:1/4\nK:Amin\n!mf!ABcd|e3c|e3A|cAFc|A4-|AdcB|e3c|e3A|cAFc|A4|]"
#endif
