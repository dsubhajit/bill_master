#ifndef ADDNEWEQUIPMENTS_H
#define ADDNEWEQUIPMENTS_H

#include <QDialog>

namespace Ui {
class AddNewEquipments;
}

class AddNewEquipments : public QDialog
{
    Q_OBJECT

public:
    explicit AddNewEquipments(QWidget *parent = 0);
    ~AddNewEquipments();

private:
    Ui::AddNewEquipments *ui;
};

#endif // ADDNEWEQUIPMENTS_H
