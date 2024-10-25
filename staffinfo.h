#ifndef STAFFINFO_H
#define STAFFINFO_H

#include <QWidget>
#include <QTreeWidgetItem>
namespace Ui {
class StaffInfo;
}

class StaffInfo : public QWidget
{
    Q_OBJECT

public:
    explicit StaffInfo(QWidget *parent = 0);
    ~StaffInfo();
    void loadStaff();
    Ui::StaffInfo *ui;

    void reloadPayments();

    void calculatePayment(int arType);

private slots:
    void on_addStaff_clicked();

    void on_deleteStaff_clicked();

    void on_editStaffDetails_clicked();

    void on_addPayment_clicked();

    void on_staffListTree_itemDoubleClicked(QTreeWidgetItem *item, int column);

    void on_paymentFilter_clicked();

    void on_pushButton_2_clicked();

    void on_showTotalStaffPayment_clicked();

private:

};

#endif // STAFFINFO_H
