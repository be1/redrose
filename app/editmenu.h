#ifndef EDITMENU_H
#define EDITMENU_H

#include <QMenu>
#include "AbcPlainTextEdit.h"

class EditMenu : public QMenu
{
    Q_OBJECT

public:
    EditMenu(QWidget* parent = nullptr);
    ~EditMenu();

protected slots:
    void onFindActivated();
    void onFindForwardActivated();
    void onFindBackwardActivated();

private:
    AbcPlainTextEdit* getCurrentEditor();
    QString m_text;
};

#endif // EDITMENU_H
