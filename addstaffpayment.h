#ifndef ADDSTAFFPAYMENT_H
#define ADDSTAFFPAYMENT_H

#include <QDialog>
#include "staffinfo.h"
namespace Ui {
class AddStaffPayment;
}

class AddStaffPayment : public QDialog
{
    Q_OBJECT

public:
    explicit AddStaffPayment(QWidget *parent = 0);

    int staff_id;
    StaffInfo *mStaffInfoParent;
    void setStaffId(int arId);
    void setStafInfoParent(StaffInfo *arP);

    ~AddStaffPayment();

private slots:
    void on_cancelPayment_clicked();

    void on_savePayment_clicked();

private:
    Ui::AddStaffPayment *ui;
};

#endif // ADDSTAFFPAYMENT_H
