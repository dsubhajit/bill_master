#ifndef ROOMSETTINGS_H
#define ROOMSETTINGS_H

#include <QWidget>

namespace Ui {
class RoomSettings;
}

class RoomSettings : public QWidget
{
    Q_OBJECT

public:
    explicit RoomSettings(QWidget *parent = 0);
    void addAllRoomTypes();
    void addAllRoomEquipments();
    ~RoomSettings();

private slots:
    void on_closeBtn_clicked();

    void on_addRoomType_clicked();

    void on_addEquip_clicked();

    void on_deleteRoom_clicked();

    void on_deleteEquip_clicked();

private:
    Ui::RoomSettings *ui;
};

#endif // ROOMSETTINGS_H
