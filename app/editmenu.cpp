#include "editmenu.h"
#include <QInputDialog>
#include "AbcApplication.h"
#include "EditWidget.h"

EditMenu::EditMenu(QWidget* parent)
    : QMenu(parent)
{
    setTitle(tr("Edit"));

    addAction(tr("Find ..."), this, SLOT(onFindActivated()), QKeySequence::Find);
    addAction(tr("Find forward"), this, SLOT(onFindForwardActivated()), QKeySequence::FindNext);
    addAction(tr("Find backward"), this, SLOT(onFindBackwardActivated()), QKeySequence::FindPrevious);
}

EditMenu::~EditMenu()
{

}

AbcPlainTextEdit* EditMenu::getCurrentEditor()
{
    AbcApplication* a = static_cast<AbcApplication*>(qApp);
    QWidget* cw = a->mainWindow()->mainHSplitter()->editTabWidget()->currentWidget();

    if (cw)
        return static_cast<EditWidget*>(cw)->editVBoxLayout()->abcPlainTextEdit();

    return nullptr;
}
void EditMenu::onFindActivated()
{
    AbcPlainTextEdit* editor = getCurrentEditor();
    if (!editor)
        return;

    m_text = QInputDialog::getText(editor, tr("Find ..."), tr("Text:"));
    if (m_text.isEmpty())
        return;

    editor->find(m_text);
}

void EditMenu::onFindForwardActivated()
{
    AbcPlainTextEdit* editor = getCurrentEditor();
    if (!editor)
        return;

    if (m_text.isEmpty())
        m_text = QInputDialog::getText(editor, tr("Find forward"), tr("Text:"));

    if (m_text.isEmpty())
        return;

    editor->find(m_text);
}

void EditMenu::onFindBackwardActivated()
{
    AbcPlainTextEdit* editor = getCurrentEditor();
    if (!editor)
        return;

    if (m_text.isEmpty())
        m_text = QInputDialog::getText(editor, tr("Find backward"), tr("Text:"));

    if (m_text.isEmpty())
        return;

    editor->find(m_text, QTextDocument::FindBackward);
}

