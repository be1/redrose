#include "editorprefdialog.h"
#include <QVBoxLayout>
#include <QPushButton>
#include <QColorDialog>
#include "config.h"
#include "settings.h"

EditorPrefDialog::EditorPrefDialog(QWidget *parent) : QDialog(parent)
{
    Settings settings;

    setWindowTitle(tr("Editor settings"));
    setMinimumSize(400, 320);

    mainLayout = new QVBoxLayout;

    bool highlight = settings.value(EDITOR_HIGHLIGHT).toBool();
    highlightLabel = new QLabel(tr("Highlight current line"));
    highlightCheck = new QCheckBox;
    highlightCheck->setChecked(highlight);
    highlightLabel->setBuddy(highlightCheck);
    QHBoxLayout* hbox = new QHBoxLayout;
    hbox->addWidget(highlightLabel);
    hbox->addWidget(highlightCheck);

    mainLayout->addLayout(hbox);

    /* WARNING: labels, keys, buttons in the same order */
    colorLabels << tr("Header color") \
                << tr("Comment color") \
                << tr("Extra instruction color") \
                << tr("Measure bar color") \
                << tr("Decoration color") \
                << tr("Guitar chord color") \
                << tr("Lyric color");

    colorKeys << EDITOR_HEADER_COLOR \
              << EDITOR_COMMENT_COLOR \
              << EDITOR_EXTRAINSTR_COLOR \
              << EDITOR_BAR_COLOR \
              << EDITOR_DECORATION_COLOR \
              << EDITOR_GCHORD_COLOR \
              << EDITOR_LYRIC_COLOR;


    for (int i = 0; i < colorKeys.length(); i++) {
        QColor color = settings.value(colorKeys.at(i)).toString();
        QHBoxLayout* hbox = new QHBoxLayout;
        QLabel* label = new QLabel(colorLabels.at(i));
        QPushButton* butt = new QPushButton;
        QPalette pal = butt->palette();
        pal.setColor(QPalette::Button, color);
        pal.setColor(QPalette::ButtonText, color);
        label->setBuddy(butt);
        butt->setText(QVariant(color).toString());

        butt->setStyleSheet("color: " + QVariant(color).toString() + ";");
        butt->setPalette(pal);

        butt->setAutoFillBackground(true);
        butt->update();
        colorButtons.append(butt);
        connect(butt, &QPushButton::clicked, this, &EditorPrefDialog::onColorButtonClicked);
        hbox->addWidget(label);
        hbox->addWidget(butt);
        mainLayout->addLayout(hbox);
    }

    buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
    mainLayout->addWidget(buttons);
    setLayout(mainLayout);
}

EditorPrefDialog::~EditorPrefDialog()
{
    delete mainLayout;
}

QColor EditorPrefDialog::getColor(QString key)
{
    int i = colorKeys.indexOf(key);
    QPushButton* butt = colorButtons.at(i);
    QPalette pal = butt->palette();
    QColor color = pal.color(QPalette::Button);
    return color;
}

bool EditorPrefDialog::getHighlight()
{
    return highlightCheck->isChecked();
}

void EditorPrefDialog::onColorButtonClicked()
{
   QPushButton* butt = static_cast<QPushButton*>(sender());
   QPalette pal = butt->palette();
   QColor color = pal.color(QPalette::Button);

   QColor choose = QColorDialog::getColor(color, this);

   if (!choose.isValid())
       return;

   pal.setColor(QPalette::Button, choose);
   pal.setColor(QPalette::ButtonText, choose);
   butt->setAutoFillBackground(true);

   butt->setStyleSheet("color: " + QVariant(choose).toString() + ";");
   butt->setPalette(pal);

   butt->setText(QVariant(choose).toString());
   butt->update();
}
