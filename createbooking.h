#include "bookingcalendar.h"
#include <QDate>
#ifndef CREATEBOOKING_H
#define CREATEBOOKING_H

#include <QDialog>

namespace Ui {
class CreateBooking;
}

class CreateBooking : public QDialog
{
    Q_OBJECT

public:
    BookingCalendar* mpParent;
    explicit CreateBooking(QWidget *parent = 0);
    bool mEditMode;
    int mBookingId;
    QDate mBookingFrom;
    QDate mBookingTo;

    ~CreateBooking();
    void addRoomsInForm();
    void addCustomersInForm();
    bool checkRoomAvailability();
    int  genBookingId();
    int m_customer_id;
    void addStatusFields();
    bool checkBookingAvail(QString date_from,QString date_to,int room_id);

    void setParent(BookingCalendar* aclndr);
    void setEditMode(bool mode);
    void setBookingId(int id);

private slots:


    //void on_roomsList_currentIndexChanged(int index);

    void on_from_date_dateChanged(const QDate &date);

    void on_to_date_dateChanged(const QDate &date);
    void onItemActivated(const QModelIndex &index);




    //void on_test_editingFinished();

    void on_addNewCustomer_clicked();



    void on_cancelBtn_clicked();

    void on_saveBookingBtn_clicked();



    void on_checkAvail_clicked();

    void on_customer_status_currentIndexChanged(int index);

    void on_multiple_day_clicked(bool checked);

    void on_single_day_clicked(bool checked);



    void on_payment_status_currentIndexChanged(int index);

private:
    Ui::CreateBooking *ui;
};

#endif // CREATEBOOKING_H
