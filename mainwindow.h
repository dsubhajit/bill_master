#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "customers.h"
#include "rooms.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    Ui::MainWindow *ui;
    void clearRenderArea();
    void reloadRooms();
    ~MainWindow();

private slots:
    void on_actionCustomers_triggered();

    void on_actionRoom_Settings_triggered();

    void on_actionRooms_triggered();


    void on_actionBooking_Calendar_triggered();

    void on_actionFood_triggered();

    void on_actionLog_Book_triggered();

    void on_actionInvoices_triggered();

    void on_actionHome_triggered();

    void on_actionAbout_triggered();

    void on_actionHotel_Information_triggered();

    void on_actionStatistics_triggered();

    void on_actionAccounts_triggered();

    void on_actionChange_Password_triggered();

    void on_actionExit_triggered();

    void on_actionStaff_Details_Payments_triggered();

    void on_actionGST_triggered();

private:
   // Ui::MainWindow *ui;
    Customers mCustomers;
    Rooms mRooms;

//  Functions



};

#endif // MAINWINDOW_H
