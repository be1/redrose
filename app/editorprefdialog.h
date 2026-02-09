#ifndef EDITORPREFDIALOG_H
#define EDITORPREFDIALOG_H

#include <QDialog>
#include <QVBoxLayout>
#include <QDialogButtonBox>
#include <QLabel>
#include <QCheckBox>
#include <QSpinBox>
#include <QFontComboBox>

class EditorPrefDialog : public QDialog
{
    Q_OBJECT
public:
    explicit EditorPrefDialog(QWidget* parent = nullptr);
    ~EditorPrefDialog();

    QColor getColor(QString key);
    bool getHighlight();
    bool getFollow();
    bool getAutoplay();
    int getFontRange();
    QFont getBaseFont();

signals:

private slots:
    void onColorButtonClicked();

private:
    QVBoxLayout* mainLayout;

    QLabel* fontLabel;
    QFontComboBox* fontBaseCombo;

    QLabel* fontRangeLabel;
    QSpinBox* fontRangeSpinBox;

    QLabel* highlightLabel;
    QCheckBox* highlightCheck;

    QLabel* followLabel;
    QCheckBox* followCheck;

    QLabel* autoplayLabel;
    QCheckBox* autoplayCheck;

    QStringList colorLabels;
    QStringList colorKeys;
    QList<QPushButton*> colorButtons;

    QDialogButtonBox* buttons;
};

#endif // EDITORPREFDIALOG_H
