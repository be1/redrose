#ifndef WIZARD_H
#define WIZARD_H

#include <QDialog>
#include <QLineEdit>
#include <QSpinBox>
#include <QCheckBox>
#include <QComboBox>
#include <QDialogButtonBox>

class Wizard : public QDialog
{
    Q_OBJECT
public:
    Wizard(QWidget* parent = nullptr);

    const QString title(const QString& deflt);
    const QString composer(const QString &deflt);
    int voices();
    QString braceType();

private:
    QLineEdit* m_title;
    QLineEdit* m_composer;
    QSpinBox* m_voices;
    QComboBox* m_bracetype;
    QDialogButtonBox* buttons;
};

#endif // WIZARD_H
