#ifndef BOOKINGCALENDAR_H
#define BOOKINGCALENDAR_H

#include <QWidget>

namespace Ui {
class BookingCalendar;
}

class BookingCalendar : public QWidget
{
    Q_OBJECT

public:
    explicit BookingCalendar(QWidget *parent = 0);
    void createCalendar();
    void createCalendar(QDate aFromDate,QDate aToDate);

    void createBookingLogTable();
    void populateBookingLog();
    ~BookingCalendar();

private slots:
    void on_createBooking_clicked();
    void on_bookingLogTable_cellDoubleClicked(int row, int column);
    void on_deleteBooking_clicked();
    void on_calendarFilter_clicked();

    void on_bookingLogFilter_clicked();

    void on_printBookingConfirm_clicked();

    void on_checkOutBtn_clicked();

    void on_createInvoice_clicked();

    void on_searchButton_clicked();

    void on_editBooking_clicked();

    void on_advPaymentBtn_clicked();

private:
    Ui::BookingCalendar *ui;
};

#endif // BOOKINGCALENDAR_H
