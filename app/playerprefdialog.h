#ifndef PLAYERPREFDIALOG_H
#define PLAYERPREFDIALOG_H

#include <QLabel>
#include <QComboBox>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QDialogButtonBox>
#include <QDialog>

class PlayerPrefDialog : public QDialog
{
    Q_OBJECT
public:
    PlayerPrefDialog(QWidget* parent = nullptr);

    QString getPlayer();
    QString getDriver();
    QString getSoundfont();
    int getVelocity();
    int getDuration();
    double getVolume();

#if 0
    double getReverb();
#endif
protected slots:
    void onSoundfontButtonClicked();
    void onPlayerComboChanged(const QString& text);

private:
    QVBoxLayout* mainLayout;

    QLabel* playerLabel;
    QComboBox* playerComboBox;

    QLabel* driverLabel;
    QComboBox* driverComboBox;

    QLabel* soundfontLabel;
    QPushButton* soundfontButton;
    QString soundfont;

    QLabel* defaultVelocityLabel;
    QSpinBox* defaultVelocitySpinBox;

    QLabel* defaultDurationLabel;
    QSpinBox* defaultDurationSpinBox;

    QLabel* volumeLabel;
    QDoubleSpinBox* volumeDoubleSpinBox;

#if 0
    QLabel* reverbLabel;
    QDoubleSpinBox* reverbDoubleSpinBox;
#endif
    QDialogButtonBox* buttons;
};

#endif // PLAYERPREFDIALOG_H
