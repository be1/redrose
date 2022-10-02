#ifndef EDITORPREFDIALOG_H
#define EDITORPREFDIALOG_H

#include <QDialog>
#include <QVBoxLayout>
#include <QDialogButtonBox>
#include <QLabel>
#include <QCheckBox>
#include <QSpinBox>

class EditorPrefDialog : public QDialog
{
    Q_OBJECT
public:
    explicit EditorPrefDialog(QWidget *parent = nullptr);
    ~EditorPrefDialog();

    QColor getColor(QString key);
    bool getHighlight();
    bool getAutoplay();
    int getAutoplayChan();
    int getFontRange();

signals:

private slots:
    void onColorButtonClicked();

private:
    QVBoxLayout* mainLayout;

    QLabel* fontRangeLabel;
    QSpinBox* fontRangeSpinBox;

    QLabel* highlightLabel;
    QCheckBox* highlightCheck;

    QLabel* autoplayLabel;
    QCheckBox* autoplayCheck;

    QLabel* autochanLabel;
    QSpinBox* autochanSpinBox;

    QStringList colorLabels;
    QStringList colorKeys;
    QList<QPushButton*> colorButtons;

    QDialogButtonBox* buttons;
};

#endif // EDITORPREFDIALOG_H
