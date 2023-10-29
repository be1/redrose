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
    double getReverb();

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

    QLabel* reverbLabel;
    QDoubleSpinBox* reverbDoubleSpinBox;

    QDialogButtonBox* buttons;
};

#endif // PLAYERPREFDIALOG_H
