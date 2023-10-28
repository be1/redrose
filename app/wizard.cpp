#include "wizard.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QSpinBox>
#include <QCheckBox>
#include <QComboBox>

Wizard::Wizard(QWidget *parent) :
    QDialog(parent)
{
    setWindowTitle(tr("New score"));
    setMinimumSize(400, 320);

    QVBoxLayout* mainLayout = new QVBoxLayout;

    m_title = new QLineEdit;
    m_title->setPlaceholderText(tr("Title"));
    m_subtitle = new QLineEdit;
    m_subtitle->setPlaceholderText(tr("Subtitle"));
    m_composer = new QLineEdit;
    m_composer->setPlaceholderText(tr("Composer / Lyricist"));

    QHBoxLayout* hbox1 = new QHBoxLayout;
    QLabel* label1 = new QLabel;
    label1->setText(tr("Voices:"));
    m_voices = new QSpinBox;
    m_voices->setMinimum(1);
    label1->setBuddy(m_voices);
    hbox1->addWidget(label1);
    hbox1->addWidget(m_voices);

    QHBoxLayout* hbox2 = new QHBoxLayout;
    QLabel* label2 = new QLabel;
    label2->setText(tr("Brace type:"));
    m_bracetype = new QComboBox;
    m_bracetype->addItem(tr("none"), "");
    m_bracetype->addItem(tr("Brace {"), "{");
    m_bracetype->addItem(tr("Bracket ["), "[");
    label2->setBuddy(m_bracetype);
    hbox2->addWidget(label2);
    hbox2->addWidget(m_bracetype);

    buttons = new QDialogButtonBox(QDialogButtonBox::Ok);
    connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);

    mainLayout->addWidget(m_title);
    mainLayout->addWidget(m_subtitle);
    mainLayout->addWidget(m_composer);
    mainLayout->addLayout(hbox1);
    mainLayout->addLayout(hbox2);
    mainLayout->addWidget(buttons);
    setLayout(mainLayout);
}

const QString Wizard::title()
{
    return m_title->text();
}

const QString Wizard::subtitle()
{
    return m_subtitle->text();
}

const QString Wizard::composer()
{
    return m_composer->text();
}

int Wizard::voices()
{
    return m_voices->value();
}

QString Wizard::braceType()
{
    return m_bracetype->currentData().toString();
}
