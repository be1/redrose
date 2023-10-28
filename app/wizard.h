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
    enum Template { TemplateNone, TemplateKeyboard, TemplateString4tet, TemplateSATBChoir };

    const QString title(const QString& deflt);
    const QString composer(const QString &deflt);
    int voices();
    QString grouping();
    Template templat();

private slots:
    void onTemplateChange();

private:
    QLineEdit* m_title;
    QLineEdit* m_composer;
    QSpinBox* m_voices;
    QComboBox* m_grouping;
    QComboBox* m_template;
    QDialogButtonBox* buttons;
};

#endif // WIZARD_H
