#ifndef ADDROOMS_H
#define ADDROOMS_H

#include <QWidget>
#include "rooms.h"

namespace Ui {
class AddRooms;
}

class AddRooms : public QWidget
{
    Q_OBJECT

public:
    explicit AddRooms(QWidget *parent = 0);
    void setEditMode(bool eMode);
    void setRoomId(int roomId);
    void addAllRoomEquipments();
    void addAllRoomTypes();
    void setRoomParent(Rooms* arm);
    ~AddRooms();

private slots:
    void on_cancelAddNewRoom_clicked();



    void on_saveRoom_clicked();

private:
    Rooms *mRooms;
    Ui::AddRooms *ui;
    bool mEditMode;
    int mRoomId;
};

#endif // ADDROOMS_H
