#ifndef EDITORPREFDIALOG_H
#define EDITORPREFDIALOG_H

#include <QDialog>
#include <QVBoxLayout>
#include <QDialogButtonBox>
#include <QLabel>
#include <QCheckBox>

class EditorPrefDialog : public QDialog
{
    Q_OBJECT
public:
    explicit EditorPrefDialog(QWidget *parent = nullptr);
    ~EditorPrefDialog();

    QColor getColor(QString key);
    bool getHighlight();

signals:

private slots:
    void onColorButtonClicked();

private:
    QVBoxLayout* mainLayout;

    QLabel* highlightLabel;
    QCheckBox* highlightCheck;

    QStringList colorLabels;
    QStringList colorKeys;
    QList<QPushButton*> colorButtons;

    QDialogButtonBox* buttons;
};

#endif // EDITORPREFDIALOG_H
