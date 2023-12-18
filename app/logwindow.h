#ifndef LOGWINDOW_H
#define LOGWINDOW_H

#include <QWidget>
#include <QVBoxLayout>
#include <QTextEdit>
#include <QPushButton>

class LogWindow : public QWidget
{
public:
    LogWindow(QWidget* parent = nullptr);
    ~LogWindow();

    void clear();
    void append(const QString& text);

private:
    QVBoxLayout* m_layout;
    QTextEdit* m_textedit;
    QPushButton* m_clearbutton;
};

#endif // LOGWINDOW_H
