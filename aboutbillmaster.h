#ifndef ABOUTBILLMASTER_H
#define ABOUTBILLMASTER_H

#include <QWidget>

namespace Ui {
class AboutBillMaster;
}

class AboutBillMaster : public QWidget
{
    Q_OBJECT

public:
    explicit AboutBillMaster(QWidget *parent = 0);
    ~AboutBillMaster();

private:
    Ui::AboutBillMaster *ui;
};

#endif // ABOUTBILLMASTER_H
