#include "logwindow.h"

LogWindow::LogWindow(QWidget *parent)
    : QWidget(parent)
{
    m_layout = new QVBoxLayout;
    m_textedit = new QTextEdit;
    m_clearbutton = new QPushButton;

    connect(m_clearbutton, &QPushButton::clicked, m_textedit, &QTextEdit::clear);

    m_textedit->setReadOnly(true);
    m_textedit->setAcceptRichText(true);

    m_clearbutton->setText(tr("Clear Log"));

    m_layout->addWidget(m_textedit, 1);
    m_layout->addWidget(m_clearbutton, 0, Qt::AlignHCenter);

    setLayout(m_layout);
}

LogWindow::~LogWindow()
{
    delete m_layout;
}

void LogWindow::clear()
{
    m_textedit->clear();
}

void LogWindow::append(const QString& text)
{
    m_textedit->append(text);
}
