#ifndef PLAYERPREFDIALOG_H
#define PLAYERPREFDIALOG_H

#include <QLabel>
#include <QComboBox>
#include <QSpinBox>
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

    QLabel* defaultVelocityLabel;
    QSpinBox* defaultVelocitySpinBox;

    QLabel* defaultDurationLabel;
    QSpinBox* defaultDurationSpinBox;

    QDialogButtonBox* buttons;
};

#endif // PLAYERPREFDIALOG_H
