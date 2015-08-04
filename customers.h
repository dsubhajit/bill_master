#ifndef CUSTOMERS_H
#define CUSTOMERS_H

#include <QWidget>
#include <QtSql>
#include <QDebug>
#include "createbooking.h"
#include "dbmysql.h"

namespace Ui {
class Customers;
}

class Customers : public QWidget
{
    Q_OBJECT

public:

    explicit Customers(QWidget *parent = 0);
    ~Customers();
    void getAllCustomerData();

private slots:
    //void on_toolButton_triggered(QAction *arg1);

    void on_addCustomer_clicked();

    void on_editCustomer_clicked();

    void on_deleteCustomer_clicked();

    void on_searchBtn_clicked();

private:
    Ui::Customers *ui;
//  Functions


};




#endif // CUSTOMERS_H
