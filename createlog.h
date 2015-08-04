#include "logbook.h"
#ifndef CREATELOG_H
#define CREATELOG_H
#include "bookingcalendar.h"

#include <QDialog>

namespace Ui {
class CreateLog;
}

class CreateLog : public QDialog
{
    Q_OBJECT

public:
    explicit CreateLog(QWidget *parent = 0);
    int mBookingId;
    BookingCalendar* mBookingCal;

    LogBook* mLogBookObj;
    int mLogId;
    void setDataFields(int booking_id);
    bool mEditMode;
    bool checkIfRoomNotVacant(QString room_ids);
    void setLogBookObject(LogBook* aObj);
    void setBookingCalObject(BookingCalendar* aObj);
    void setEditMode(bool editmode, int logId);
    ~CreateLog();

private slots:
    void on_cancelBtn_clicked();

    void on_saveBtn_clicked();

private:
    Ui::CreateLog *ui;
};

#endif // CREATELOG_H
