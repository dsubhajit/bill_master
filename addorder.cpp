#include "addorder.h"
#include "ui_addorder.h"

AddOrder::AddOrder(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddOrder)
{
    ui->setupUi(this);
}

AddOrder::~AddOrder()
{
    delete ui;
}
