#ifndef ABCPLAINTEXTEDIT_H
#define ABCPLAINTEXTEDIT_H

#include <QPlainTextEdit>
#include <QSyntaxHighlighter>
#include <QRegularExpression>
#include <QCompleter>

class AbcHighlighter : public QSyntaxHighlighter
{
    Q_OBJECT

public:
    AbcHighlighter(QTextDocument *parent = 0);

protected:
    void highlightBlock(const QString &text) override;

private:
    struct AbcHighlightingRule
    {
        QRegularExpression pattern;
        QTextCharFormat format;
    };

    QVector<AbcHighlightingRule> highlightingRules;

#if 0
    QRegularExpression headerStartExpression;
    QRegularExpression headerEndExpression;
#endif
    QTextCharFormat headerFormat;
    QTextCharFormat extraInstructionFormat;
    QTextCharFormat singleLineCommentFormat;
    QTextCharFormat noteFormat;
    QTextCharFormat decorFormat;
    QTextCharFormat gchordFormat;
    QTextCharFormat barFormat;
    QTextCharFormat lyricFormat;
#if 0
    QTextCharFormat multiLineHeaderFormat;
#endif
};

class AbcPlainTextEdit: public QPlainTextEdit
{
	Q_OBJECT

public:
	AbcPlainTextEdit(QWidget* parent = nullptr);
    ~AbcPlainTextEdit();

    void lineNumberAreaPaintEvent(QPaintEvent *event);
    int lineNumberAreaWidth();

    void setCompleter(QCompleter *c);
    QCompleter *completer() const;

    bool isSaved();
    void setSaved();


protected:
    void resizeEvent(QResizeEvent *event) override;
    void keyPressEvent(QKeyEvent *e) override;
    void focusInEvent(QFocusEvent *e) override;
    void flagModified(bool enable);

private slots:
    void updateLineNumberAreaWidth(int newBlockCount);
    void highlightCurrentLine();
    void checkDictionnary();
    void updateLineNumberArea(const QRect &rect, int dy);
    void insertCompletion(const QString &completion);

private:
    QAbstractItemModel *modelFromFile(const QString &fileName);
    QAbstractItemModel *dictModel; /* normal dictionnary */
    QAbstractItemModel *gmModel;   /* General MIDI dictionary */

    QWidget *lineNumberArea;
    AbcHighlighter *highlighter;
    QString textUnderCursor() const;
    QString lineUnderCursor() const;

    QCompleter *c = nullptr;
    bool saved;
};

class LineNumberArea : public QWidget
{
public:
    LineNumberArea(AbcPlainTextEdit *editor) : QWidget(editor), abcplaintextedit(editor)
    {}

    QSize sizeHint() const override
    {
        return QSize(abcplaintextedit->lineNumberAreaWidth(), 0);
    }

protected:
    void paintEvent(QPaintEvent *event) override
    {
        abcplaintextedit->lineNumberAreaPaintEvent(event);
    }

private:
    AbcPlainTextEdit *abcplaintextedit;
};

#endif
