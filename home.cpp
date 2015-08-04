#include "home.h"
#include "ui_home.h"
#include "dbmysql.h"
#include <QMessageBox>
#include <QPushButton>

Home::Home(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Home)
{

    ui->setupUi(this);
    createRooms();
    createCheckOutTable();
    createTodaysBookingTable();
}

Home::~Home()
{
    delete ui;
}


bool Home::checkIfBookingExists(int roomId)
{
    DbMysql* d = DbMysql::getInstance();
    QMessageBox msgBox;

    if(!d->getConnection().open())
    {
        msgBox.critical(this,"Error","Failed to connect database.1");
        return false;
    }
    else
    {

        QString curDate = QDate::currentDate().toString("yyyy-MM-dd");

        QSqlQuery query1( "SELECT room_bookings.room_id FROM `room_bookings` where room_id="+QString::number(roomId)+" and '"+curDate+"' = DATE_ADD(booking_to,INTERVAL 1 DAY) ;" ,d->getConnection());
        if(!query1.isActive())
        {

            msgBox.critical(this,"Error","Failed to connect database.");
            return false;
        }
        else
        {

            qDebug()<<query1.lastQuery();
            if(query1.size() > 0) return true;
            else return false;
        }
    }
}

/*
bool Home::checkIfCheckedOut(int roomId)
{
    DbMysql* d = DbMysql::getInstance();
    QMessageBox msgBox;

    if(!d->getConnection().open())
    {
        msgBox.critical(this,"Error","Failed to connect database.1");
        return false;
    }
    else
    {

        QString curDate = QDate::currentDate().toString("yyyy-MM-dd");

        QSqlQuery query1( "SELECT * FROM booking_ where room_id="+QString::number(roomId)+" and DATE_ADD(booking.booking_to,INTERVAL 1 DAY) = CURDATE() and booking_log.check_out_time is null;" ,d->getConnection());
        if(!query1.isActive())
        {
            msgBox.critical(this,"Error","Failed to connect database.");
            return false;
        }
        else
        {
            //qDebug()<<query1.lastQuery();
            if(query1.size() == 0) return true;
            else return false;
        }
    }
}
*/


void Home::createRooms(){
    DbMysql* d = DbMysql::getInstance();
    QMessageBox msgBox;

    if(!d->getConnection().open())
    {
        msgBox.critical(this,"Error","Failed to connect database.1");
    }
    else
    {

        QSqlQuery query1( "select room_id,room_number,room_state,room_type from rooms order by room_id;" ,d->getConnection());
        if(!query1.isActive())
        {
            msgBox.critical(this,"Error","Failed to connect database.");
        }
        else
        {

            int pos = 0;
            int count = 0;
            int col = 0;            
            while(query1.next())
            {
                QPushButton *room_btn = new QPushButton(query1.value(1).toString());
                room_btn->setText(query1.value(1).toString());
                //ui->roomsList->addItem(query1.value(1).toString(),QVariant::fromValue(query1.value(0).toInt()));
                room_btn->setMaximumWidth(150);
                room_btn->setMaximumHeight(150);
                room_btn->setMinimumWidth(100);
                if(query1.value(2).toInt() == 0)
                {
                    room_btn->setText(query1.value(1).toString()+"\n"+query1.value("room_type").toString()+"\nVacant");
                    room_btn->setStyleSheet("QPushButton { font-size: 15pt;font-weight: bold;background: #138637; }");
                }
                else {
                    if(checkIfBookingExists(query1.value(0).toInt())){
                        room_btn->setText(query1.value(1).toString()+"\n"+query1.value("room_type").toString()+"\nCheckout\nToday");
                        room_btn->setStyleSheet("QPushButton { font-size: 15pt;font-weight: bold;background: #F90; }");
                    }
                    else {
                        room_btn->setText(query1.value(1).toString()+"\n"+query1.value("room_type").toString()+"\nOccupied");
                        room_btn->setStyleSheet("QPushButton { font-size: 15pt;font-weight: bold;background: #FF0000; }");
                    }
                }

                //qDebug()<<pos<<"  "<<col;
                ui->gridLayout_2->addWidget(room_btn,pos,col,1,1);
                count++; col++;
                if(count%4==0){
                    col = 0;
                    pos++;
                }
                //vlay->addWidget(room_btn);

            }
        }
    }
}

void Home::createCheckOutTable()
{
    DbMysql* d = DbMysql::getInstance();
    QMessageBox msgBox;

    if(!d->getConnection().open())
    {
        msgBox.critical(this,"Error","Failed to connect database.1");
    }
    else
    {

        QSqlQuery query1( "SELECT booking_id,customer_name,room_numbers,booking_from,booking_to,reg_serial FROM booking_details where DATE_ADD(booking_to,INTERVAL 1 DAY) = CURDATE() order by booking_id desc",d->getConnection());
        qDebug()<<query1.lastQuery();
        if(!query1.isActive())
        {
            msgBox.critical(this,"Error","Failed to connect database.");
        }
        else
        {
            ui->checkOutTable->setRowCount(query1.size());
            int row = 0 ;

            while(query1.next())
            {

                int columnSize = ui->checkOutTable->columnCount();
                qDebug()<<"Column Size:"<<columnSize;

                for( int column = 0; column < columnSize; column++)
                {
                    QTableWidgetItem *newItem = new QTableWidgetItem();
                    newItem->setData(Qt::UserRole,QVariant::fromValue(query1.value(0).toInt()));

                    //newItem->setData(Qt::,QVariant::fromValue(query1.value(columnSize).toInt()));
                    newItem->setText(query1.value(column).toString());
                    ui->checkOutTable->setItem(row, column, newItem);

                }

                row++;
            }
            //ui->checkOutTable->resizeColumnsToContents();
            ui->checkOutTable->horizontalHeader()->setHighlightSections(false);
            ui->checkOutTable->horizontalHeader()->setStyleSheet("QHeaderView::section { background-color:#0B2161;color:#fff; }");

        }
    }
}

void Home::createTodaysBookingTable()
{
    DbMysql* d = DbMysql::getInstance();
    QMessageBox msgBox;

    if(!d->getConnection().open())
    {
        msgBox.critical(this,"Error","Failed to connect database.1");
    }
    else
    {

        QSqlQuery query1( "SELECT booking_id,customer_name,room_numbers,booking_from,booking_to,reg_serial FROM booking_details where booking_from ='"+QDate::currentDate().toString("yyyy-MM-dd")+"' order by booking_id desc",d->getConnection());
        qDebug()<<query1.lastQuery();
        if(!query1.isActive())
        {
            msgBox.critical(this,"Error","Failed to connect database.");
        }
        else
        {
            ui->todaysBooking->setRowCount(query1.size());
            int row = 0 ;

            while(query1.next())
            {

                int columnSize = ui->todaysBooking->columnCount();
                qDebug()<<"Column Size:"<<columnSize;

                for( int column = 0; column < columnSize; column++)
                {
                    QTableWidgetItem *newItem = new QTableWidgetItem();
                    newItem->setData(Qt::UserRole,QVariant::fromValue(query1.value(0).toInt()));

                    //newItem->setData(Qt::,QVariant::fromValue(query1.value(columnSize).toInt()));
                    newItem->setText(query1.value(column).toString());
                    ui->todaysBooking->setItem(row, column, newItem);

                }

                row++;
            }
            //ui->checkOutTable->resizeColumnsToContents();
            ui->todaysBooking->horizontalHeader()->setHighlightSections(false);
            ui->todaysBooking->horizontalHeader()->setStyleSheet("QHeaderView::section { background-color:#0B2161;color:#fff; }");

        }
    }
}
