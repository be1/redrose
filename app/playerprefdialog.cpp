#include <QVBoxLayout>
#include <QFileDialog>

#include "config.h"
#include "playerprefdialog.h"
#include "AbcApplication.h"
#include "settings.h"

PlayerPrefDialog::PlayerPrefDialog(QWidget *parent): QDialog(parent)
{
    Settings settings;

    setWindowTitle(tr("Player settings"));
    setMinimumSize(400, 320);

    mainLayout = new QVBoxLayout;

    /* MIDI generator (aka. player) */
    QVariant player = settings.value(PLAYER_KEY);
    playerLabel = new QLabel(tr("MIDI generator"));
    playerComboBox = new QComboBox;
    playerComboBox->setEditable(true);

    QStringList pitems;
    if (!player.isNull()) {
        pitems << player.toString();
    }

    pitems << ABC2MIDI << LIBABC2SMF;
    pitems.removeDuplicates();

    playerComboBox->addItems(pitems);
    connect(playerComboBox, &QComboBox::currentTextChanged, this, &PlayerPrefDialog::onPlayerComboChanged);
    playerLabel->setBuddy(playerComboBox);
    QHBoxLayout* plhbox = new QHBoxLayout;
    plhbox->addWidget(playerLabel);
    plhbox->addWidget(playerComboBox);

    mainLayout->addLayout(plhbox);

    /* velocity */
    int velocity = settings.value(PLAYER_VELOCITY).toInt();
    defaultVelocityLabel = new QLabel(tr("Default MIDI velocity"));
    defaultVelocitySpinBox = new QSpinBox;
    defaultVelocitySpinBox->setRange(0,127);
    defaultVelocitySpinBox->setValue(velocity);

    if (player.toString() != LIBABC2SMF)
        defaultVelocitySpinBox->setEnabled(false);

    defaultVelocityLabel->setBuddy(defaultVelocitySpinBox);
    QHBoxLayout* vlhbox = new QHBoxLayout;
    vlhbox->addWidget(defaultVelocityLabel);
    vlhbox->addWidget(defaultVelocitySpinBox);

    mainLayout->addLayout(vlhbox);

    /* duration */
    int duration = settings.value(PLAYER_DURATION).toInt();
    if (duration <= 0)
        duration = 1;

    defaultDurationLabel = new QLabel(tr("Default Relative Note duration"));
    defaultDurationSpinBox = new QSpinBox;
    defaultDurationSpinBox->setRange(1,10);
    defaultDurationSpinBox->setValue(duration);

    if (player.toString() != LIBABC2SMF)
        defaultDurationSpinBox->setEnabled(false);

    defaultDurationLabel->setBuddy(defaultVelocitySpinBox);
    QHBoxLayout* drhbox = new QHBoxLayout;
    drhbox->addWidget(defaultDurationLabel);
    drhbox->addWidget(defaultDurationSpinBox);

    mainLayout->addLayout(drhbox);


    /* driver */
    QVariant driver = settings.value(DRIVER_KEY);
    driverLabel = new QLabel(tr("Audio driver"));
    driverComboBox = new QComboBox;

    QStringList ditems;
    if (!driver.isNull()) {
        ditems << driver.toString();
    }

    ditems << DRIVER_ALSA << DRIVER_OSS << DRIVER_PULSEAUDIO << DRIVER_JACK;
    ditems.removeDuplicates();

    driverComboBox->addItems(ditems);
    driverLabel->setBuddy(driverComboBox);
    QHBoxLayout* dvhbox = new QHBoxLayout;
    dvhbox->addWidget(driverLabel);
    dvhbox->addWidget(driverComboBox);

    mainLayout->addLayout(dvhbox);

    /* soundfont */
    QVariant font = settings.value(SOUNDFONT_KEY);
    soundfontLabel = new QLabel(tr("Audio sound font"));

    QString text = tr("Choose file");
    if (!font.isNull()) {
        text = font.toString();
        soundfont = text;
    }

    soundfontButton = new QPushButton(text);
    connect(soundfontButton, &QPushButton::clicked, this, &PlayerPrefDialog::onSoundfontButtonClicked);
    soundfontLabel->setBuddy(soundfontButton);
    QHBoxLayout* sfhbox = new QHBoxLayout;
    sfhbox->addWidget(soundfontLabel);
    sfhbox->addWidget(soundfontButton);

    mainLayout->addLayout(sfhbox);
#if 0
    /* reverb */
    QVariant reverb = settings.value(REVERB_KEY);
    reverbLabel = new QLabel(tr("Reverb level"));
    reverbDoubleSpinBox = new QDoubleSpinBox;
    reverbDoubleSpinBox->setMaximum(1.);
    reverbDoubleSpinBox->setMinimum(0.);
    reverbDoubleSpinBox->setStepType(QDoubleSpinBox::AdaptiveDecimalStepType);
    reverbDoubleSpinBox->setValue(reverb.toDouble());
    reverbLabel->setBuddy(reverbDoubleSpinBox);
    QHBoxLayout* revhbox = new QHBoxLayout;
    revhbox->addWidget(reverbLabel);
    revhbox->addWidget(reverbDoubleSpinBox);

    mainLayout->addLayout(revhbox);
#endif
    /* OK / Cancel buttons */
    buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
    mainLayout->addWidget(buttons);
    setLayout(mainLayout);
}

QString PlayerPrefDialog::getPlayer()
{
    return playerComboBox->currentText();
}

QString PlayerPrefDialog::getDriver()
{
    return driverComboBox->currentText();
}

QString PlayerPrefDialog::getSoundfont()
{
    return soundfont;
}

int PlayerPrefDialog::getVelocity()
{
    return defaultVelocitySpinBox->value();
}

int PlayerPrefDialog::getDuration()
{
    return defaultDurationSpinBox->value();
}

#if 0
double PlayerPrefDialog::getReverb()
{
    return reverbDoubleSpinBox->value();
}
#endif
void PlayerPrefDialog::onSoundfontButtonClicked()
{
    AbcApplication* a = static_cast<AbcApplication*>(qApp);

    QString sf;
    if (!soundfont.isNull()) {
        QFileInfo info(soundfont);
        sf = QFileDialog::getOpenFileName(a->mainWindow(), tr("Audio sound font selection"), info.absolutePath(), tr("Soundfont (*.sf[23])"));
    } else {
        sf = QFileDialog::getOpenFileName(a->mainWindow(), tr("Audio sound font selection"), QDir::homePath(), tr("Soundfont (*.sf[23])"));
    }

    if (sf.isNull())
        return;

    soundfont = sf;
    soundfontButton->setText(sf);
}

void PlayerPrefDialog::onPlayerComboChanged(const QString& text)
{
    bool enable = (text == LIBABC2SMF);
    defaultVelocitySpinBox->setEnabled(enable);
    defaultDurationSpinBox->setEnabled(enable);
}
