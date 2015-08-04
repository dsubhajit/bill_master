#ifndef ADDADVANCEPAYMENT_H
#define ADDADVANCEPAYMENT_H

#include <QDialog>

namespace Ui {
class AddAdvancePayment;
}

class AddAdvancePayment : public QDialog
{
    Q_OBJECT

public:
    explicit AddAdvancePayment(QWidget *parent = 0);
    int mBookingId;
    void setBookingId(int bookingId);
    void getAllPayments();
    ~AddAdvancePayment();

private slots:
    void on_addPayment_clicked();

    void on_deletePayment_clicked();

private:
    Ui::AddAdvancePayment *ui;
};

#endif // ADDADVANCEPAYMENT_H
