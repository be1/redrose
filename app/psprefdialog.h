#ifndef PSPREFDIALOG_H
#define PSPREFDIALOG_H

#include <QVBoxLayout>
#include <QLabel>
#include <QComboBox>
#include <QDialogButtonBox>

#include <QDialog>

class PsPrefDialog : public QDialog
{
    Q_OBJECT
public:
    PsPrefDialog(QWidget* parent = nullptr);
    QString getTunesExport();
    int getPageNumbering();

private:
    QVBoxLayout* m_mainLayout;
    QLabel* m_tunesLabel;
    QComboBox* m_tunesComboBox;
    QLabel* m_pagesLabel;
    QComboBox* m_pagesComboBox;
    QDialogButtonBox* m_buttons;
};

#endif // PSPREFDIALOG_H
