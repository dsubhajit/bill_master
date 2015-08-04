#ifndef ADD_NEW_CUSTOMER_H
#define ADD_NEW_CUSTOMER_H

#include <QWidget>
#include "customers.h"

namespace Ui {
class add_new_customer;
}

class add_new_customer : public QWidget
{
    Q_OBJECT

public:
    void setBookingCreateParent(CreateBooking *Obj);
    CreateBooking *m_cb;
    bool mIsCb;
    explicit add_new_customer(QWidget *parent = 0);
    void editCustomer(QString aCustomerId);
    Customers *mCustomer;
    ~add_new_customer();
    void setCustomerParent(Customers *parent);

private slots:


    void on_cancelAddNewCustomertn_clicked();

    void on_saveNewCustomer_clicked();



private:
    Ui::add_new_customer *ui;
};

#endif // ADD_NEW_CUSTOMER_H
