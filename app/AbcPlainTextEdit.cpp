#include "AbcPlainTextEdit.h"
#include <QDebug>
#include <QFile>
#include <QMenu>
#include <QInputDialog>
#include <QPainter>
#include <QTextBlock>
#include <QAbstractItemView>
#include <QAbstractItemModel>
#include <QScrollBar>
#include <QStringListModel>
#include <QGuiApplication>
#include "config.h"
#include "settings.h"

const QString AbcPlainTextEdit::delimiter = " !%~@#$^&*()_+{}|:\"<>?,./;'[]\\-=";

AbcPlainTextEdit::AbcPlainTextEdit(QWidget* parent)
    : QPlainTextEdit(parent),
      saved(false),
      autoplay(false)
{
    lineNumberArea = new LineNumberArea(this);
    highlighter = new AbcHighlighter(this->document());
    dictModel = modelFromFile(":dict.txt");
    psModel = modelFromFile(":ps.txt");
    gmModel = modelFromFile(":gm.txt");

    QCompleter* com = new QCompleter(this);
    com->setModel(dictModel);
    com->setModelSorting(QCompleter::CaseInsensitivelySortedModel);
    com->setCaseSensitivity(Qt::CaseInsensitive);
    com->setWrapAround(false);
    setCompleter(com);

    Settings settings;

    connect(this, &AbcPlainTextEdit::blockCountChanged, this, &AbcPlainTextEdit::updateLineNumberAreaWidth1);
    connect(this, &AbcPlainTextEdit::cursorPositionChanged, this, &AbcPlainTextEdit::updateLineNumberAreaWidth0);
    connect(this, &AbcPlainTextEdit::updateRequest, this, &AbcPlainTextEdit::updateLineNumberArea);
    connect(this, &AbcPlainTextEdit::cursorPositionChanged, this, &AbcPlainTextEdit::checkDictionnary);
    connect(this, &AbcPlainTextEdit::modificationChanged, this, &AbcPlainTextEdit::flagModified);
#ifndef NEW_AUTOPLAY
    if (settings.value(EDITOR_AUTOPLAY).toBool()) {
        autoplay = true;
        connect(this, &AbcPlainTextEdit::cursorPositionChanged, this, &AbcPlainTextEdit::checkPlayableNote);
    }
#endif
    updateLineNumberAreaWidth0();

    QVariant fontBase = settings.value(EDITOR_FONT_BASE);
    QFont base;
    base.setFamily(fontBase.toString());
    setFont(base);

    QVariant fontRange = settings.value(EDITOR_FONT_RANGE);
    int range = fontRange.toInt();
    zoomIn(range);

    QVariant enableHighlightCurrentLine = settings.value(EDITOR_HIGHLIGHT);
    if (enableHighlightCurrentLine.toBool()) {
        connect(this, &AbcPlainTextEdit::cursorPositionChanged, this, &AbcPlainTextEdit::highlightCurrentLine);
        highlightCurrentLine();
    }

    /* custom actions */
    findaction = new QAction(tr("Find..."), this);
    findaction->setShortcut(QKeySequence::Find);
    findaction->setShortcutContext(Qt::WidgetWithChildrenShortcut);
    connect(findaction, &QAction::triggered, this, &AbcPlainTextEdit::onFindActivated);
    addAction(findaction);

    findnextaction = new QAction(tr("Find forward"), this);
    findnextaction->setShortcut(QKeySequence::FindNext);
    findnextaction->setShortcutContext(Qt::WidgetWithChildrenShortcut);
    connect(findnextaction, &QAction::triggered, this, &AbcPlainTextEdit::onFindForwardActivated);
    addAction(findnextaction);

    findprevaction = new QAction(tr("Find backward"), this);
    findprevaction->setShortcut(QKeySequence::FindPrevious);
    findprevaction->setShortcutContext(Qt::WidgetWithChildrenShortcut);
    connect(findprevaction, &QAction::triggered, this, &AbcPlainTextEdit::onFindBackwardActivated);
    addAction(findprevaction);
}

AbcPlainTextEdit::~AbcPlainTextEdit()
{
    delete dictModel;
    delete psModel;
    delete gmModel;
}

void AbcPlainTextEdit::setPlainText(const QString& plaintext)
{
    QPlainTextEdit::setPlainText(plaintext);
    emit plainTextSet();
}

void AbcPlainTextEdit::setCompleter(QCompleter* completer)
{
    if (c)
        c->disconnect(this);

    c = completer;

    if (!c)
        return;

    c->setWidget(this);
    c->setCompletionMode(QCompleter::PopupCompletion);
    c->setFilterMode(Qt::MatchContains);
    QObject::connect(c, QOverload<const QString&>::of(&QCompleter::activated),
                     this, &AbcPlainTextEdit::insertCompletion);
}

QCompleter* AbcPlainTextEdit::completer() const
{
    return c;
}

void AbcPlainTextEdit::flagModified(bool enable)
{
    this->saved = !enable;
}

void AbcPlainTextEdit::contextMenuEvent(QContextMenuEvent* e)
{
    QMenu* menu = createStandardContextMenu(e->globalPos());
    menu->setParent(this);
    menu->addAction(findaction);
    menu->addAction(findnextaction);
    menu->addAction(findprevaction);
    menu->exec(e->globalPos());
    delete menu;
}

void AbcPlainTextEdit::mouseDoubleClickEvent(QMouseEvent* e)
{
    QTextDocument* doc = document();
    QTextCursor tc = textCursor();

    if (charBeforeCursor(tc) == ']' || charBeforeCursor(tc) == '|') {
        tc.movePosition(QTextCursor::Left, QTextCursor::MoveAnchor, 2);
    }

    /* postion at start of measure */
    QTextCursor lbar = doc->find(QRegularExpression(QStringLiteral("(\\||^)")), tc, QTextDocument::FindBackward);
    tc.setPosition(lbar.position());

    /* select right part: until end of measure */
    QTextCursor rbar = doc->find(QRegularExpression(QStringLiteral("(\\||$)")), tc);
    tc.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor, rbar.position() - tc.position());

    /* default to full line */
    if (tc.selectedText().isEmpty()) {
        QTextCursor sol = doc->find(QRegularExpression("^"), tc, QTextDocument::FindBackward);
        tc.setPosition(sol.position());
        QTextCursor eol = doc->find(QRegularExpression("$"), tc);
        tc.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor, eol.position() - sol.position());
    }

    setTextCursor(tc);
    e->accept();
}

bool AbcPlainTextEdit::isSaved()
{
    return this->saved;
}

void AbcPlainTextEdit::setSaved()
{
    this->saved = true;
    this->document()->setModified(false);
}

QString AbcPlainTextEdit::constructHeaders(int selectionIndex, int* x)
{
    QString headers;

    QString all = this->toPlainText();
    int i = 0, xl = 0;
    QStringList lines = all.split('\n');

    /* find last X: before selectionIndex */
    for (int l = 0; l < lines.count() && i < selectionIndex; l++) {
        i += lines.at(l).size() +1; /* count \n */
        if (lines.at(l).startsWith("X:")) {
            /* report X value */
#if (QT_VERSION < QT_VERSION_CHECK(6,0,0))
            *x = lines.at(l).midRef(2).toInt();
#else
            *x = lines.at(l).mid(2).toInt();
#endif
            xl = l;
            /* don't break on first X: continue until selectionIndex */
        }
    }

    /* construct headers */
    for (int j = xl;  j < lines.count(); j++) {
        /* stop at 'V:' but include all other headers and comments above it */
        if (lines.at(j).contains(QRegularExpression("^((%[^\n]*)|([A-UW-Z]:[^\n]*))$"))) {
            headers += lines.at(j) + "\n";
        } else /* found 'V:' or notes */
            break;
    }

    return headers;
}

void AbcPlainTextEdit::insertCompletion(const QString& completion)
{
    if (c->widget() != this)
        return;
    QTextCursor tc = textCursor();
#if 0
    /* this for startWith mode */
    int extra = completion.length() - c->completionPrefix().length();
    tc.movePosition(QTextCursor::Left);
    tc.movePosition(QTextCursor::EndOfWord);
    tc.insertText(completion.right(extra));
#else
    /* this for contains mode */
    while (tc.movePosition(QTextCursor::Left, QTextCursor::KeepAnchor)) {
        if (delimiter.contains(tc.selectedText().at(0))) {
            tc.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor);
            break;
        }
    }
    tc.removeSelectedText();
    tc.insertText(completion);
#endif
    setTextCursor(tc);
}

void AbcPlainTextEdit::onFindActivated()
{
    if (!textCursor().selectedText().isEmpty())
        m_find = textCursor().selectedText();

    m_find = QInputDialog::getText(this, tr("Find..."), tr("Text:"), QLineEdit::Normal, m_find);
    if (m_find.isEmpty())
        return;

    if(!find(m_find, QTextDocument::FindCaseSensitively))
        find(m_find, QTextDocument::FindBackward|QTextDocument::FindCaseSensitively);
}

void AbcPlainTextEdit::onFindForwardActivated()
{
    if (m_find.isEmpty())
        m_find = textCursor().selectedText();

    if (m_find.isEmpty())
            m_find = QInputDialog::getText(this, tr("Find forward"), tr("Text:"));

    if (m_find.isEmpty())
        return;

    find(m_find, QTextDocument::FindCaseSensitively);
}

void AbcPlainTextEdit::onFindBackwardActivated()
{
    if (m_find.isEmpty())
        m_find = textCursor().selectedText();

    if (m_find.isEmpty())
            m_find = QInputDialog::getText(this, tr("Find backward"), tr("Text:"));

    if (m_find.isEmpty())
        return;

    find(m_find, QTextDocument::FindBackward|QTextDocument::FindCaseSensitively);
}

QString AbcPlainTextEdit::textUnderCursor() const
{
    QTextCursor tc = textCursor();
    tc.select(QTextCursor::WordUnderCursor);
    return tc.selectedText();
}

QString AbcPlainTextEdit::lineUnderCursor() const
{
    QTextCursor tc = textCursor();
    tc.select(QTextCursor::LineUnderCursor);
    return tc.selectedText();
}

QString AbcPlainTextEdit::charBeforeCursor(QTextCursor tc) const
{
    if (!tc.movePosition(QTextCursor::Left))
        return QString();

    tc.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor);
    return tc.selectedText();
}

QString AbcPlainTextEdit::wordBeforeCursor(QTextCursor tc) const
{
    /* start of word delimiter */
    while (tc.movePosition(QTextCursor::Left, QTextCursor::KeepAnchor)) {
        if (delimiter.contains(tc.selectedText().at(0))) {
            tc.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor);
            break;
        }
    }

    return tc.selectedText();
}

bool AbcPlainTextEdit::isRest(QChar car) const
{
    return car == 'z' || car == 'Z' || car == 'x' || car == 'X';
}

bool AbcPlainTextEdit::isPitch(QChar car) const
{
    return (car >= 'A' && car <= 'G') || (car >= 'a' && car <= 'g');
}

bool AbcPlainTextEdit::isAccid(QChar car) const
{
    return car == '^' || car == '=' || car == '_';
}

void AbcPlainTextEdit::checkDictionnary(void) {
    QString line = lineUnderCursor();
    if (c && (c->model() != gmModel) &&
            (line.startsWith("%%MIDI program") || line.startsWith("%%MIDI bassprog") || line.startsWith("%%MIDI chordprog"))) {
        c->setModel(gmModel);
    } else if (c && (c->model() != psModel) &&
               (line.startsWith("%%"))) {
        c->setModel(psModel);
    } else if (c && (c->model() != dictModel) && cursorIsInFragmentLine()) {
        c->setModel(dictModel);
    } else if (c) {
        c->setModel(nullptr);
    }
}

QString AbcPlainTextEdit::noteUnderCursor(QTextCursor tc) const
{
    QString sym = charBeforeCursor(tc);

    if (sym.isEmpty())
        return QString();

    if (!isPitch(sym.at(0)))
        return QString();

    /* now, it is a pitch */

    /* find measure accidentals for this pitch */
    QTextCursor bar = textCursor();

    while (bar.movePosition(QTextCursor::Left, QTextCursor::KeepAnchor, 1)) {
        /* run until beginning of measure */
        if (bar.selectedText().at(0) == '|' || bar.selectedText().at(0) == '\n' || bar.selectedText().at(0) == QChar::ParagraphSeparator)
            break;

        /* find same pitch in previous notes (+/- octavas) */
        if (bar.selectedText().at(0).toUpper() == sym.at(0).toUpper()) {
            bar.clearSelection();
            if (!bar.movePosition(QTextCursor::Left, QTextCursor::KeepAnchor, 1)) {
                /* start of buffer: no left move */
                break;
            }

            bool found = false;
            /* get accidentals */
            while (isAccid(bar.selectedText().at(0))) {
                found = true;
                sym.prepend(bar.selectedText().at(0));
                bar.clearSelection();
                if(!bar.movePosition(QTextCursor::Left, QTextCursor::KeepAnchor, 1)) {
                    /* start of buffer: no left move */
                    break;
                }
            }

            if (found)
                /* parsing finished */
                break;
            else {
                bar.clearSelection();
                bar.movePosition(QTextCursor::Right);
            }
        }
    }

    /* octavas */
    while (tc.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor, 1)) {
        QString oct = tc.selectedText();
        QChar last = oct.at(oct.size() -1);
        if (last != ',' && last != '\'')
            break;

        sym.append(last);
    }

    return sym;
}

QString AbcPlainTextEdit::getLastKeySignatureChange() const
{
    QTextDocument* doc = document();
    QTextCursor tc, vtc, ktc;

    vtc =  doc->find(QRegularExpression("^V:"), textCursor(), QTextDocument::FindBackward|QTextDocument::FindCaseSensitively);

    /* search inline change */
    tc = doc->find("[K:", textCursor(), QTextDocument::FindBackward|QTextDocument::FindCaseSensitively);
    /* give KS only if it changed after "V:" */
    if (tc.position() > vtc.position()) {
        tc.movePosition(QTextCursor::Right);
        tc.select(QTextCursor::WordUnderCursor);
        return "[K:" + tc.selectedText() + "]";
    }

    /* search also for non-inline change */
    tc = doc->find(QRegularExpression("^K:"), textCursor(), QTextDocument::FindBackward|QTextDocument::FindCaseSensitively);
    ktc = doc->find(QRegularExpression("^K:"), tc, QTextDocument::FindBackward|QTextDocument::FindCaseSensitively);
    /* we must avoid initial KS */
    if (tc.position() > ktc.position() && tc.position() > vtc.position()) {
        tc.select(QTextCursor::LineUnderCursor);
        return tc.selectedText() + "\n";
    }

    return QString();
}

QString AbcPlainTextEdit::getLastMidiProgramChange() const
{
    QTextDocument* doc = document();
    QTextCursor tc, vtc;

    vtc =  doc->find(QRegularExpression("^V:"), textCursor(), QTextDocument::FindBackward|QTextDocument::FindCaseSensitively);

    /* search %%MIDI program N */
    tc = doc->find("%%MIDI program", textCursor(), QTextDocument::FindBackward|QTextDocument::FindCaseSensitively);
    if (tc.position() > vtc.position()) {
        tc.select(QTextCursor::LineUnderCursor);
        return tc.selectedText() + "\n";
    }

    return QString();
}

bool AbcPlainTextEdit::gotoX(int x)
{
    QRegularExpression re("^X:[ \\t]*" + QString::number(x) + "[ \\t]*$");
    QTextCursor tc = document()->find(re);
    if (tc.isNull())
        return false;

    tc.clearSelection();
    /* go to end of area */
    verticalScrollBar()->setValue(verticalScrollBar()->maximum());
    /* go up */
    setTextCursor(tc);
    return true;
}

int AbcPlainTextEdit::lastX()
{
    int val = 0;
    bool ok = false;
    QRegularExpression re("X:[ \\t]*([\\d]+)"); /* dunno why "^X" fails. Use "X" */
    QRegularExpressionMatch match;
    int i = toPlainText().lastIndexOf(re, -1, &match);
    if (i < 0) {
        qWarning() << "could not find last X of text";
        return val;
    }

    QString last = match.capturedTexts().last();
    val = last.toInt(&ok);
    if (!ok)
        qWarning() << "unparseable match for last X of text";

    return val;
}

bool AbcPlainTextEdit::cursorIsInFragmentLine()
{
    QString line = lineUnderCursor();

    /* comment */
    if (line.startsWith('%'))
        return false;

    /* formulated header */
    if (line.size() > 1 && line.at(0).isLetter() && line.at(1) == ':')
        return false;

    /* in progress header or garbage */
    if (line.size() > 0 && line.at(0).isLetter() && !isPitch(line.at(0)) && !isRest(line.at(0)))
        return false;

    /* anything else */
    return true;
}

void AbcPlainTextEdit::setTextCursorPosition(int n) {
    QTextCursor c = textCursor();
    int prevpos = c.position();
#if 0
    int uni_index = n;
    QByteArray ba;
    QTextCursor cursor = textCursor();

    /* a bruteforce way to match real index when there are utf-8 chars */
    do {
        cursor.movePosition(QTextCursor::Start);
        cursor.movePosition(QTextCursor::MoveOperation::Right, QTextCursor::KeepAnchor, uni_index);

        /* when coming from QTextCursor::selectedText(), LF is replaced by U+2029! */
        QString text = cursor.selectedText().replace(QChar::ParagraphSeparator, "\n");
        ba = text.toUtf8();

        if (ba.size() == n)
            break;

        uni_index--;
    } while (ba.size() > n);
#endif

    if (n > prevpos) {
        c.movePosition(QTextCursor::MoveOperation::Right, QTextCursor::MoveAnchor, n - prevpos);
        setTextCursor(c);
    } else if (n < prevpos) {
        c.movePosition(QTextCursor::MoveOperation::Left, QTextCursor::MoveAnchor, prevpos - n);
        setTextCursor(c);
    }

    /* do absolutely nothing if position looks the same */
}

/* returns the V:... text or %%MIDI channel... text */
QString AbcPlainTextEdit::getCurrentVoiceOrChannel(bool prefer_voice) const
{
    QTextCursor tc;
    QTextDocument* doc = document();

    QTextCursor xtc = doc->find(QRegularExpression("^X:"), textCursor(), QTextDocument::FindBackward|QTextDocument::FindCaseSensitively);
    QTextCursor vtc =  doc->find(QRegularExpression("^V:"), textCursor(), QTextDocument::FindBackward|QTextDocument::FindCaseSensitively);
    QTextCursor ctc = doc->find(QRegularExpression("^%%MIDI channel "), textCursor(), QTextDocument::FindBackward|QTextDocument::FindCaseSensitively);
    if (xtc.position() > vtc.position())
        /* if X is before V, the V found belongs to the previous tune
         * so, now check our V forward */
        vtc = doc->find(QRegularExpression("^V:"), textCursor(), QTextDocument::FindCaseSensitively);

    if (ctc.position() > vtc.position() && !prefer_voice)
        tc = ctc;
    else
        tc = vtc;

    /* check if tc was valid */
    if (tc.position() > 0) {
        tc.select(QTextCursor::LineUnderCursor);
        return tc.selectedText() + "\n";
    } else {
        return QString();
    }
}

QString AbcPlainTextEdit::getCurrentMIDIComment(const QString& com) const
{
    QTextCursor tc;
    QTextDocument* doc = document();
    QTextCursor vtc =  doc->find(QRegularExpression("^V:"), textCursor(), QTextDocument::FindBackward|QTextDocument::FindCaseSensitively);
    tc =  doc->find(QString("%%MIDI %1 ").arg(com), textCursor(), QTextDocument::FindBackward|QTextDocument::FindCaseSensitively);

    tc.select(QTextCursor::LineUnderCursor);
    if (tc.position() > vtc.position())
        return tc.selectedText() + "\n";

    return QString();
}

static QString matchUnderCursor(QTextCursor tc, QRegularExpression re) {
    tc.select(tc.LineUnderCursor);
    QString line = tc.selectedText();
    QRegularExpressionMatch match = re.match(line);
    QStringList m = match.capturedTexts();
    /* do not report whole match (0) if there is a specific one */
    if (m.length())
        return m.last();

    return QString();
}

int AbcPlainTextEdit::currentXV(char xv)
{
    QTextCursor xtc = document()->find(QRegularExpression("^X:"), textCursor(), QTextDocument::FindBackward|QTextDocument::FindCaseSensitively);
    if (xtc.isNull())
        return 1; /* X or V are 1 */

    if (xv == 'X') {
        QRegularExpression re("^X:[ \\t]*(\\d*)");
        QString x = matchUnderCursor(xtc, re);
        bool ok;
        int ret = x.toInt(&ok);
        if (ok)
            return ret;
        qDebug() << "could not find current X";
        return 1;
    }

    /* V header lookup */
    QTextCursor vtc = document()->find(QRegularExpression("^V:"), textCursor(), QTextDocument::FindBackward|QTextDocument::FindCaseSensitively);
    if (vtc.position() > xtc.position()) {
        QRegularExpression re("^V:[ \\t]*(\\d*)");
        QString v = matchUnderCursor(vtc, re);
        bool ok;
        int ret = v.toInt(&ok);
        if (ok)
            return ret;
        qDebug () << "could not find current V";
        return 1;
    }

    return 1;
}

QString AbcPlainTextEdit::playableNoteUnderCursor(QTextCursor tc)
{
    /* check manual selection */
    if (!tc.selectedText().isEmpty())
        return QString();

    QString line = lineUnderCursor();

    /* check if nothing under cursor */
    if (line.isEmpty())
         return QString();

    /* check if we are in a comment */
    if (line.startsWith("%"))
        return QString();

    /* check if we are in a header line */
    if (line.at(0).isLetter()) {
        if (line.size() > 1)
            if (line.at(1) == ':')
                return QString();
    }

    /* check if we are in !something! or in "GChord" */
    QTextCursor check = textCursor();
    while (check.movePosition(QTextCursor::Left, QTextCursor::KeepAnchor, 1)) {
        /* go until start of line */
        if (check.selectedText().at(0) == '\n' || check.selectedText().at(0) == QChar::ParagraphSeparator)
            break;
    }
    if (check.selectedText().count('!') % 2)
        return QString();

    if (check.selectedText().count('"') % 2)
        return QString();

    /* check if we are in a InlineChange or a chord */
    check = textCursor();
    int colon = 0;
    int bracket = 0;
    while (check.movePosition(QTextCursor::Left, QTextCursor::KeepAnchor, 1)) {
        if (check.selectedText().at(0) == ']') {
            bracket = 0;
            break;
        }

        if (check.selectedText().at(0) == ':') {
            colon = check.position();
        }

        if (check.selectedText().at(0) == '[') {
            bracket = check.position();
            break;
        }

        if (check.selectedText().at(0) == '\n' || check.selectedText().at(0) == QChar::ParagraphSeparator) {
            break;
        }
    }

    if (colon && bracket && colon > bracket) {
        /* [Inline:Change]  */
         return QString();
    }

    /* check if this is a note */
    QString note = noteUnderCursor(tc);
    if (note.isEmpty())
        return QString();

    QString voice = getCurrentVoiceOrChannel(true);
    QString keysig = getLastKeySignatureChange();
    QString pgm = getCurrentMIDIComment("program");
    QString trp = getCurrentMIDIComment("transpose");

    note.prepend(trp);
    note.prepend(pgm);
    note.prepend(keysig);
    note.prepend(voice);

    return note;
}

void AbcPlainTextEdit::checkPlayableNote()
{
    QString note = playableNoteUnderCursor(textCursor());
    if (note.isEmpty())
        return;

    emit playableNote(note);
}

void AbcPlainTextEdit::focusInEvent(QFocusEvent* e)
{
    if (c)
        c->setWidget(this);
    QPlainTextEdit::focusInEvent(e);
}

void AbcPlainTextEdit::keyPressEvent(QKeyEvent* e)
{
    /* avoid reaction when some text is selected */
    if (!textCursor().selectedText().isEmpty())
        return QPlainTextEdit::keyPressEvent(e);

    /* avoid reaction on selecting whole text with kbd shortcut */
    if (e->modifiers().testFlag(Qt::ControlModifier) && QChar::toUpper(e->key()) == 'A')
        return QPlainTextEdit::keyPressEvent(e);

    /* bad hack to play octavied input note */
    if (autoplay && (e->key() == ',' || e->key() == '\'')) {
        QPlainTextEdit::keyPressEvent(e);

        QTextCursor tc = textCursor();
        QChar c = charBeforeCursor(tc).at(0);
        while (c == ',' || c == '\'') {
            if (!tc.movePosition(QTextCursor::MoveOperation::Left))
                break;

            c = charBeforeCursor(tc).at(0);
        }

        QString note = playableNoteUnderCursor(tc);
        if (!note.isEmpty())
            emit playableNote(note);

        return;
    }

    /* completions stuff */
    if (c && c->popup()->isVisible()) {
        // The following keys are forwarded by the completer to the widget
       switch (e->key()) {
       case Qt::Key_Enter:
       case Qt::Key_Return:
       case Qt::Key_Escape:
       case Qt::Key_Tab:
       case Qt::Key_Backtab:
            e->ignore();
            return; // let the completer do default behavior
       default:
           break;
       }
    }

    /* Here, the completer popup is not shown yet */

    //const bool isShortcut = (e->modifiers().testFlag(Qt::ControlModifier) && e->key() == Qt::Key_E); // CTRL+E
    const bool isShortcut = e->key() == Qt::Key_Tab && e->modifiers().testFlag(Qt::NoModifier);
    if (!c || !isShortcut)
        QPlainTextEdit::keyPressEvent(e);

    const bool ctrlOrShift = e->modifiers().testFlag(Qt::ControlModifier) ||
                             e->modifiers().testFlag(Qt::ShiftModifier);
    if (!c || (ctrlOrShift && e->text().isEmpty()))
        return;

    const bool hasModifier = (e->modifiers() != Qt::NoModifier) && !ctrlOrShift;
    QString completionPrefix = wordBeforeCursor(textCursor());

    /* no shortcut pressed, or a modifier-only key is pressed,
     * or the word typed is too short, or it is a complete word:
     * then unshow popup if needed and return */
    if (!isShortcut && (hasModifier || e->text().isEmpty() || completionPrefix.length() < 2
                      || delimiter.contains(e->text().right(1)))) {
        c->popup()->hide();
        return;
    }

    /* here we have a possible case of showing completion popup */
    if (completionPrefix != c->completionPrefix()) {
        c->setCompletionPrefix(completionPrefix);
        c->popup()->setCurrentIndex(c->completionModel()->index(0, 0));
    }
    QRect cr = cursorRect();
    cr.setWidth(c->popup()->sizeHintForColumn(0)
                + c->popup()->verticalScrollBar()->sizeHint().width());
    c->complete(cr); // pop it up!
}

int AbcPlainTextEdit::lineNumberAreaWidth()
{
    int digits = 1;
    int max = qMax(1, blockCount());
    while (max >= 10) {
        max /= 10;
        ++digits;
    }

    QTextCursor tc = textCursor();
    int maxpos = qMax(1, tc.block().text().length());
    while (maxpos >= 10) {
        maxpos /= 10;
        ++digits;
    }

    int sep = fontMetrics().horizontalAdvance(':');
    int margins = 3;
    int space = margins + sep + fontMetrics().horizontalAdvance(QLatin1Char('X')) * digits;

    return space;
}

void AbcPlainTextEdit::updateLineNumberAreaWidth0() {
    setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
}

void AbcPlainTextEdit::updateLineNumberAreaWidth1(int newBlockCount) {
    (void) newBlockCount;
    updateLineNumberAreaWidth0();
}

void AbcPlainTextEdit::updateLineNumberArea(const QRect& rect, int dy)
{
    if (dy)
        lineNumberArea->scroll(0, dy);
    else
        lineNumberArea->update(0, rect.y(), lineNumberArea->width(), rect.height());

    if (rect.contains(viewport()->rect()))
        updateLineNumberAreaWidth0();
}

void AbcPlainTextEdit::resizeEvent(QResizeEvent* e)
{
    QPlainTextEdit::resizeEvent(e);

    QRect cr = contentsRect();
    lineNumberArea->setGeometry(QRect(cr.left(), cr.top(), lineNumberAreaWidth(), cr.height()));
}

void AbcPlainTextEdit::highlightCurrentLine()
{
    QList<QTextEdit::ExtraSelection> extraSelections;

    if (!isReadOnly()) {
        QTextEdit::ExtraSelection selection;

        QColor lineColor = qApp->palette().color(QPalette::Highlight);

        selection.format.setBackground(lineColor);
        selection.format.setProperty(QTextFormat::FullWidthSelection, true);
        selection.cursor = textCursor();
        selection.cursor.clearSelection();
        extraSelections.append(selection);
    }

    setExtraSelections(extraSelections);
}

void AbcPlainTextEdit::lineNumberAreaPaintEvent(QPaintEvent* event)
{
    QPainter painter(lineNumberArea);
    painter.fillRect(event->rect(), Qt::lightGray);
    QTextBlock block = firstVisibleBlock();
    int blockNumber = block.blockNumber();
    int top = qRound(blockBoundingGeometry(block).translated(contentOffset()).top());
    int bottom = top + qRound(blockBoundingRect(block).height());
    while (block.isValid() && top <= event->rect().bottom()) {
        if (block.isVisible() && bottom >= event->rect().top()) {
            QString line = QString::number(blockNumber + 1);

            QTextCursor tc = textCursor();
            if (tc.blockNumber() == blockNumber) {
                line += ":" + QString::number(tc.positionInBlock());
            }
            painter.setPen(Qt::black);
            painter.drawText(0, top, lineNumberArea->width(), fontMetrics().height(),
                             Qt::AlignRight, line);
        }

        block = block.next();
        top = bottom;
        bottom = top + qRound(blockBoundingRect(block).height());
        ++blockNumber;
    }
}

AbcHighlighter::AbcHighlighter(QTextDocument* parent)
    : QSyntaxHighlighter(parent)
{
    Settings settings;
    AbcHighlightingRule rule;

    QColor color = settings.value(EDITOR_BAR_COLOR).toString();
    barFormat.setFontWeight(QFont::Bold);
    barFormat.setForeground(color);
    rule.pattern = QRegularExpression(QStringLiteral("(:\\|*:|[:\\|\\[]?\\|[:\\|\\]]?)"));
    rule.format = barFormat;
    highlightingRules.append(rule);

    noteFormat.setFontWeight(QFont::Bold);
    noteFormat.setForeground(qApp->palette().color(QPalette::Text));
    rule.pattern = QRegularExpression(QStringLiteral("[_=^]*[A-HZa-hz][,']*[0-9]*/*[1-9]*"));
    rule.format = noteFormat;
    highlightingRules.append(rule);

    color = settings.value(EDITOR_DECORATION_COLOR).toString();
    decorFormat.setFontWeight(QFont::Normal);
    decorFormat.setForeground(color);
    rule.pattern = QRegularExpression(QStringLiteral("![^!]*!"));
    rule.format = decorFormat;
    highlightingRules.append(rule);

    color = settings.value(EDITOR_GCHORD_COLOR).toString();
    gchordFormat.setFontWeight(QFont::Normal);
    gchordFormat.setForeground(color);
    rule.pattern = QRegularExpression(QStringLiteral("\"[^\"]*\""));
    rule.format = gchordFormat;
    highlightingRules.append(rule);

    color = settings.value(EDITOR_COMMENT_COLOR).toString();
    singleLineCommentFormat.setFontWeight(QFont::Normal);
    singleLineCommentFormat.setForeground(color);
    rule.pattern = QRegularExpression(QStringLiteral("%[^\n]*"));
    rule.format = singleLineCommentFormat;
    highlightingRules.append(rule);

    color = settings.value(EDITOR_EXTRAINSTR_COLOR).toString();
    extraInstructionFormat.setFontWeight(QFont::Bold);
    extraInstructionFormat.setForeground(color);
    rule.pattern = QRegularExpression(QStringLiteral("^%%[^%\n]+"));
    rule.format = extraInstructionFormat;
    highlightingRules.append(rule);

    color = settings.value(EDITOR_LYRIC_COLOR).toString();
    lyricFormat.setFontWeight(QFont::Normal);
    lyricFormat.setForeground(color);
    rule.pattern = QRegularExpression(QStringLiteral("^[Ww]:[^\n]*"));
    rule.format = lyricFormat;
    highlightingRules.append(rule);

    color = settings.value(EDITOR_HEADER_COLOR).toString();
    headerFormat.setFontWeight(QFont::Bold);
    headerFormat.setForeground(color);
    const QString keywordPatterns[] = {
        QStringLiteral("^A:[^\n]+"), QStringLiteral("^B:[^\n]+"), QStringLiteral("^C:[^\n]+"),
        QStringLiteral("^D:[^\n]+"), QStringLiteral("^E:[^\n]+"), QStringLiteral("^F:[^\n]+"),
        QStringLiteral("^G:[^\n]+"), QStringLiteral("^H:[^\n]+"), QStringLiteral("^I:[^\n]+"),
        QStringLiteral("^K:[^\n]+"), QStringLiteral("^L:[^\n]+"), QStringLiteral("^M:[^\n]+"),
        QStringLiteral("^N:[^\n]+"), QStringLiteral("^O:[^\n]+"), QStringLiteral("^P:[^\n]+"),
        QStringLiteral("^Q:[^\n]+"), QStringLiteral("^R:[^\n]+"), QStringLiteral("^S:[^\n]+"),
        QStringLiteral("^T:[^\n]+"), QStringLiteral("^V:[^\n]+"),
        QStringLiteral("^X:[^\n]+"), QStringLiteral("^Z:[^\n]+"), QStringLiteral("\\[[KLMPQ]:[^\\]]+\\]")
    };

    for (const QString& pattern : keywordPatterns) {
        rule.pattern = QRegularExpression(pattern);
        rule.format = headerFormat;
        highlightingRules.append(rule);
    }

#if 0
    multiLineHeaderFormat.setForeground(Qt::darkMagenta);

    headerStartExpression = QRegularExpression(QStringLiteral("^X:"));
    headerEndExpression = QRegularExpression(QStringLiteral("^K:[^\n]+"));
#endif
}

void AbcHighlighter::highlightBlock(const QString& text)
{
    for (const AbcHighlightingRule& rule : std::as_const(highlightingRules)) {
        QRegularExpressionMatchIterator matchIterator = rule.pattern.globalMatch(text);
        while (matchIterator.hasNext()) {
            QRegularExpressionMatch match = matchIterator.next();
            setFormat(match.capturedStart(), match.capturedLength(), rule.format);
        }
    }
#if 0
    setCurrentBlockState(0);

    int startIndex = 0;
    if (previousBlockState() != 1)
        startIndex = text.indexOf(headerStartExpression);

    while (startIndex >= 0) {
        QRegularExpressionMatch match = headerEndExpression.match(text, startIndex);
        int endIndex = match.capturedStart();
        int headerLength = 0;
        if (endIndex == -1) {
            setCurrentBlockState(1);
            headerLength = text.length() - startIndex;
        } else {
            headerLength = endIndex - startIndex
                            + match.capturedLength();
        }
        setFormat(startIndex, headerLength, multiLineHeaderFormat);
        startIndex = text.indexOf(headerStartExpression, startIndex + headerLength);
    }
#endif
}

QAbstractItemModel* AbcPlainTextEdit::modelFromFile(const QString& fileName)
{
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly))
        return new QStringListModel(c);

#ifndef QT_NO_CURSOR
    QGuiApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
#endif
    QStringList words;

    while (!file.atEnd()) {
        QByteArray line = file.readLine();
        if (!line.isEmpty())
            words << QString::fromUtf8(line.trimmed());
    }

#ifndef QT_NO_CURSOR
    QGuiApplication::restoreOverrideCursor();
#endif
    return new QStringListModel(words, c);
}

// vim:ts=4:sw=4:et
