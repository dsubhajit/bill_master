#ifndef ADDORDER_H
#define ADDORDER_H

#include <QDialog>

namespace Ui {
class AddOrder;
}

class AddOrder : public QDialog
{
    Q_OBJECT

public:
    explicit AddOrder(QWidget *parent = 0);
    ~AddOrder();

private:
    Ui::AddOrder *ui;
};

#endif // ADDORDER_H
