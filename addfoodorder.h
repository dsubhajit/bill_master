#ifndef ADDFOODORDER_H
#define ADDFOODORDER_H

#include <QWidget>
#include "foods.h"
#include  <QModelIndex>

namespace Ui {
class AddFoodOrder;
}

class AddFoodOrder : public QWidget
{
    Q_OBJECT

public:
    Foods* m_parent;
    int booking_id;

    explicit AddFoodOrder(QWidget *parent = 0);
    void addFoodParent(Foods *parent);
    void setBookingId(int bookingId);
    void addFoodsInForm();

    void getBookingDetails();
    ~AddFoodOrder();

private slots:
    void on_cancelBtn_clicked();
    void onItemActivated(QModelIndex index);

    void on_addFoodInOrderListBtn_clicked();

    void on_saveBtn_clicked();

    void on_removeItem_clicked();

private:
    Ui::AddFoodOrder *ui;
};

#endif // ADDFOODORDER_H
