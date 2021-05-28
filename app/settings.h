#ifndef SETTINGS_H
#define SETTINGS_H

#include <QSettings>

class Settings : public QSettings
{
    Q_OBJECT
public:
    explicit Settings(QObject* parent = nullptr);
    void check();
    void reset();
signals:

};

#endif // SETTINGS_H
