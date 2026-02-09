#ifndef SCOREMENU_H
#define SCOREMENU_H

#include <QMenu>
#include <QMessageBox>
#include "EditWidget.h"
#include "wizard.h"

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
    QString strippedName(const QString& fullFileName);
    void generateTemplate(QString& abc, Wizard::Template tmpl);

    EditWidget* getCurrentEditWidget();

protected slots:
    void onQuitActionTriggered();
    void onOpenActionTriggered();
    void onOpenRecentActionTriggered();
    void onSaveActionTriggered();
    void onSaveAsActionTriggered();
    void onExportMidiActionTriggered();
    void onExportWavActionTriggered();
    void onExportPsActionTriggered();
    void onExportPdfActionTriggered();
    void onCloseActionTriggered();
    void onNewActionTriggered();

private:
    enum { MaxRecentFiles = 5 };
    QAction* recentFileActs[MaxRecentFiles];

    static const QRegularExpression m_abcext;
};

#define NEW_TEMPLATE "X:1\nT:Melody from «Le lac des cygnes»\nC:Piotr Ilitch Tchaïkovski\nM:C\nL:1/4\nK:Amin\n!mf!ABcd|e3c|e3A|cAFc|A4-|AdcB|e3c|e3A|cAFc|A4|]"
#endif
