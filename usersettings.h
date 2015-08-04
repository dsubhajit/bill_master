#ifndef USERSETTINGS_H
#define USERSETTINGS_H

#include <QDialog>

namespace Ui {
class UserSettings;
}

class UserSettings : public QDialog
{
    Q_OBJECT

public:
    explicit UserSettings(QWidget *parent = 0);
    ~UserSettings();

private slots:
    void on_saveBtn_clicked();

private:
    Ui::UserSettings *ui;
};

#endif // USERSETTINGS_H
