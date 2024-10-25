#ifndef ADDSTAFF_H
#define ADDSTAFF_H

#include <QDialog>
#include "staffinfo.h"

namespace Ui {
class AddStaff;
}

class AddStaff : public QDialog
{
    Q_OBJECT

public:
    explicit AddStaff(QWidget *parent = 0);
    void setStaffInfoParent(StaffInfo *arP);
    ~AddStaff();
    bool mEditMode;
    int staffId;
    void setEditMode(bool arMode);

    void loadStaffDataForId(int arId);

private slots:
    void on_cancelBtn_clicked();

    void on_saveStaff_clicked();

private:
    Ui::AddStaff *ui;
    StaffInfo *mStaffInfoPtr;
};

#endif // ADDSTAFF_H
