#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "ui_customers.h"
#include "bookingcalendar.h"
#include "hotelinfo.h"
#include "rooms.h"
#include "logbook.h"
#include "foods.h"
#include "invoices.h"
#include "roomsettings.h"
#include "home.h"
#include "aboutbillmaster.h"
#include "usersettings.h"
#include "stats.h"
#include "accounts.h"
#include <QtCore>
#include <QtGui>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{

    ui->setupUi(this);
    Home* hm = new Home();
    this->clearRenderArea();
    ui->mainRenderArea->addWidget(hm);
    hm->show();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::clearRenderArea()
{
    while(ui->mainRenderArea->layout()->count() > 0)
    {
        QLayoutItem *item = ui->mainRenderArea->layout()->takeAt(0);
        delete item;
    }
}

void MainWindow::on_actionCustomers_triggered()
{
    //ui->frame->set

    //mCustomers.showMaximized();
    this->clearRenderArea();
    Customers* cs = new Customers();
    cs->setAttribute(Qt::WA_DeleteOnClose);
    cs->getAllCustomerData();

    ui->mainRenderArea->addWidget(cs);

    cs->show();

}

void MainWindow::on_actionRoom_Settings_triggered()
{
    RoomSettings* rs = new RoomSettings();
    rs->setAttribute(Qt::WA_DeleteOnClose);
    rs->show();

}



void MainWindow::on_actionRooms_triggered()
{
    Rooms* rooms = new Rooms();
    this->clearRenderArea();
    rooms->getAllRoomsData();
    rooms->setAttribute(Qt::WA_DeleteOnClose);
    ui->mainRenderArea->addWidget(rooms);

    rooms->show();
}

void MainWindow::reloadRooms()
{
    Rooms* rooms = new Rooms();
    this->clearRenderArea();
    rooms->getAllRoomsData();
    rooms->setAttribute(Qt::WA_DeleteOnClose);
    ui->mainRenderArea->addWidget(rooms);

    rooms->show();
}

void MainWindow::on_actionBooking_Calendar_triggered()
{
    BookingCalendar* bc = new BookingCalendar();
    this->clearRenderArea();
    bc->createCalendar();
    bc->setAttribute(Qt::WA_DeleteOnClose);
    ui->mainRenderArea->addWidget(bc);
    bc->show();
}




void MainWindow::on_actionFood_triggered()
{
    Foods* fd = new Foods();
    this->clearRenderArea();
    fd->setAttribute(Qt::WA_DeleteOnClose);
    ui->mainRenderArea->addWidget(fd);
    fd->show();
}

void MainWindow::on_actionLog_Book_triggered()
{
    LogBook* lb = new LogBook();
    this->clearRenderArea();
    lb->setAttribute(Qt::WA_DeleteOnClose);
    ui->mainRenderArea->addWidget(lb);
    lb->show();
}

void MainWindow::on_actionInvoices_triggered()
{
    Invoices* inv = new Invoices();
    inv->setAttribute(Qt::WA_DeleteOnClose);
    this->clearRenderArea();
    ui->mainRenderArea->addWidget(inv);

    //inv->setParent(this);
    inv->show();
}

void MainWindow::on_actionHome_triggered()
{
    Home* hm = new Home();
    hm->setAttribute(Qt::WA_DeleteOnClose);
    this->clearRenderArea();
    ui->mainRenderArea->addWidget(hm);
    hm->show();
}

void MainWindow::on_actionAbout_triggered()
{
    AboutBillMaster* bm = new AboutBillMaster();
    bm->setAttribute(Qt::WA_DeleteOnClose);
    bm->show();
}

void MainWindow::on_actionHotel_Information_triggered()
{
    HotelInfo* hi  = new HotelInfo();
    hi->setAttribute(Qt::WA_DeleteOnClose);
    hi->show();
}

void MainWindow::on_actionStatistics_triggered()
{
    Stats* s = new Stats();
    s->setAttribute(Qt::WA_DeleteOnClose);
    this->clearRenderArea();
    ui->mainRenderArea->addWidget(s);
    s->show();
}

void MainWindow::on_actionAccounts_triggered()
{
    Accounts* acc = new Accounts();
    acc->setAttribute(Qt::WA_DeleteOnClose);
    this->clearRenderArea();
    ui->mainRenderArea->addWidget(acc);
    acc->show();
}

void MainWindow::on_actionChange_Password_triggered()
{
    UserSettings* us = new UserSettings();
    us->setAttribute(Qt::WA_DeleteOnClose);
    us->show();
}

void MainWindow::on_actionExit_triggered()
{
    this->close();
}
