#include "aboutbillmaster.h"
#include "ui_aboutbillmaster.h"

AboutBillMaster::AboutBillMaster(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AboutBillMaster)
{
    ui->setupUi(this);
}

AboutBillMaster::~AboutBillMaster()
{
    delete ui;
}
