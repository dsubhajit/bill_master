#ifndef ROOMS_H
#define ROOMS_H

#include <QWidget>

namespace Ui {
class Rooms;
}

class Rooms : public QWidget
{
    Q_OBJECT

public:
    explicit Rooms(QWidget *parent = 0);
    void getAllRoomsData();
    ~Rooms();

private slots:
    void on_addRooms_clicked();

    void on_search_by_room_clicked();

    void on_editRoom_clicked();

    void on_deleteRoom_clicked();

private:
    Ui::Rooms *ui;
};

#endif // ROOMS_H
