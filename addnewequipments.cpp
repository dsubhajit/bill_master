#include "addnewequipments.h"
#include "ui_addnewequipments.h"

AddNewEquipments::AddNewEquipments(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddNewEquipments)
{
    ui->setupUi(this);
}

AddNewEquipments::~AddNewEquipments()
{
    delete ui;
}
