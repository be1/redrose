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
    label2->setText(tr("Grouping:"));
    m_grouping = new QComboBox;
    m_grouping->addItem(tr("none"), "");
    m_grouping->addItem(tr("Brace {"), "{");
    m_grouping->addItem(tr("Bracket ["), "[");
    label2->setBuddy(m_grouping);
    hbox2->addWidget(label2);
    hbox2->addWidget(m_grouping);

    QHBoxLayout* hbox3 = new QHBoxLayout;
    QLabel* label3 = new QLabel;
    label3->setText(tr("Template:"));
    m_template = new QComboBox;
    m_template->addItem(tr("none"), Template::TemplateNone);
    m_template->addItem(tr("Keyboard"), Template::TemplateKeyboard);
    m_template->addItem(tr("String Quartet"), Template::TemplateString4tet);
    m_template->addItem(tr("SATB Choir"), Template::TemplateSATBChoir);
    m_template->addItem(tr("Percussion"), Template::TemplatePercussion);
    connect(m_template, &QComboBox::currentTextChanged, this, &Wizard::onTemplateChange);
    label3->setBuddy(m_template);
    hbox3->addWidget(label3);
    hbox3->addWidget(m_template);

    buttons = new QDialogButtonBox(QDialogButtonBox::Ok);
    connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);

    mainLayout->addWidget(m_title);
    mainLayout->addWidget(m_composer);
    mainLayout->addLayout(hbox1);
    mainLayout->addLayout(hbox2);
    mainLayout->addLayout(hbox3);
    mainLayout->addWidget(buttons);
    setLayout(mainLayout);
}

const QString Wizard::title(const QString &deflt)
{
    return m_title->text().isEmpty() ? deflt : m_title->text();
}

const QString Wizard::composer(const QString& deflt)
{
    return m_composer->text().isEmpty() ? deflt : m_composer->text();
}

int Wizard::voices()
{
    return m_voices->value();
}

QString Wizard::grouping()
{
    return m_grouping->currentData().toString();
}

Wizard::Template Wizard::templat()
{
    return Template(m_template->currentData().toInt());
}

void Wizard::onTemplateChange()
{
    /* assume 0 is 'none' */
    m_voices->setEnabled(!m_template->currentIndex());
    m_grouping->setEnabled(!m_template->currentIndex());
}
