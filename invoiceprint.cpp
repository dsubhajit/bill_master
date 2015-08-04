#include "invoiceprint.h"
#include "ui_invoiceprint.h"


InvoicePrint::InvoicePrint(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::InvoicePrint)
{
    ui->setupUi(this);

}

InvoicePrint::~InvoicePrint()
{
    delete ui;
}
