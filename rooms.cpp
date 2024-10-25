#include "rooms.h"
#include "addrooms.h"
#include "ui_rooms.h"
#include "addnewequipments.h"
#include "dbmysql.h"
#include <QDebug>
#include <QStyledItemDelegate>
#include <QMessageBox>

Rooms::Rooms(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Rooms)
{
    ui->setupUi(this);
}

Rooms::~Rooms()
{
    delete ui;
}

void Rooms::getAllRoomsData()
{
    DbMysql* d = DbMysql::getInstance();
    if (!d->getConnection().open()){
        qDebug() << "Failed to connect to root mysql admin";
        qDebug() << d->getConnection().lastError().text();
    }
    else {
        QSqlQuery query( "SELECT room_id,room_number, room_type, room_desc, single_beds, double_beds, extra_beds, room_equip, rate_per_room,rate_per_person,cgst,sgst,gst,room_status FROM rooms",d->getConnection() );

        if( !query.isActive() )
        {
            QMessageBox msgBox;
            msgBox.critical(this,"Error","Failed to execute query.");
            qDebug()<<"Failed to execute query. select customers";
            qDebug()<<query.lastError().text();
            return;
        }
        else{
            int row = 0;
            ui->roomDetailsTable->setRowCount(query.size());
            while( query.next() )
            {
                int columnSize = ui->roomDetailsTable->columnCount();

                for( int column = 1; column <= columnSize; column++)
                {
                    QTableWidgetItem *newItem = new QTableWidgetItem();

                    if(column == columnSize)
                    {
                        QString status ;
                        QString color;
                        switch(query.value("room_status").toInt())
                        {
                            case 1: status = "Clean Checked";color = "#5cb85c";break;
                            case 2: status = "Clean";color="#5bc0de";break;
                            case 3: status = "Need Cleaning";color="#f0ad4e";break;
                            default: status = "None";color="#d9534f";break;
                        }
                        newItem->setText(status);
                        QColor clr;
                        clr.setNamedColor(color);

                        newItem->setBackgroundColor(clr);
                    }
                    else if(column >=10 && column<=12 )
                    {
                        newItem->setText(query.value(column).toString()+" %");
                        newItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
                    }
                    else
                        newItem->setText(query.value(column).toString()); //newItem->setText(query.value(column).toString());
                    qDebug()<<query.value(column).toString();
                    newItem->setData(Qt::UserRole,QVariant::fromValue(query.value(0).toInt()));
                    ui->roomDetailsTable->setItem(row, column-1, newItem);
                }
                row++;
            }
            ui->roomDetailsTable->resizeColumnsToContents();
            //ui->roomDetailsTable->horizontalHeader()->setItemDelegate(new QStyledItemDelegate(this));
            ui->roomDetailsTable->horizontalHeader()->setHighlightSections(false);
            ui->roomDetailsTable->horizontalHeader()->setStyleSheet("QHeaderView::section { background-color:#0B2161;color:#fff; }");
        }
        d->getConnection().close();
    }
}

void Rooms::on_addRooms_clicked()
{

    AddRooms* ar = new AddRooms();
    ar->setRoomParent(this);
    ar->show();

}

void Rooms::on_search_by_room_clicked()
{
    DbMysql* d = DbMysql::getInstance();
    QMessageBox msgBox;
    if (!d->getConnection().open()){
        qDebug() << "Failed to connect to root mysql admin";
        qDebug() << d->getConnection().lastError().text();
    }
    else {
        QSqlQuery query( "SELECT room_number, room_type, room_desc, single_beds, double_beds, extra_beds, room_equip, rate_per_room,rate_per_person, room_status FROM rooms where room_number like '%"+ui->search_field->text()+"%';",d->getConnection() );

        if( !query.isActive() )
        {

            msgBox.critical(this,"Error","Failed to execute query.");
            qDebug()<<"Failed to execute query. select customers";
            qDebug()<<query.lastError().text();
            return;
        }
        else{
            int row = 0;
            if(query.size() == 0)
            {
                msgBox.information(this,"Search result","No room found.");
                return;
            }
            ui->roomDetailsTable->setRowCount(query.size());
            while( query.next() )
            {
                int columnSize = ui->roomDetailsTable->columnCount();

                for( int column = 0; column < columnSize; column++)
                {
                    QTableWidgetItem *newItem = new QTableWidgetItem();
                    newItem->setText(query.value(column).toString());
                    newItem->setData(Qt::UserRole,QVariant::fromValue(query.value(0).toInt()));
                    ui->roomDetailsTable->setItem(row, column, newItem);
                }
                row++;
            }
            ui->roomDetailsTable->resizeColumnsToContents();
            //ui->roomDetailsTable->horizontalHeader()->setItemDelegate(new QStyledItemDelegate(this));
            ui->roomDetailsTable->horizontalHeader()->setHighlightSections(false);
            ui->roomDetailsTable->horizontalHeader()->setStyleSheet("QHeaderView::section { background-color:#0B2161;color:#fff; }");
        }
    }
}

void Rooms::on_editRoom_clicked()
{
    if(ui->roomDetailsTable->selectedItems().size() > 0)
    {
        AddRooms* ar = new AddRooms();
        qDebug()<<"Room:"<<ui->roomDetailsTable->selectedItems().at(0)->data(Qt::UserRole).toInt();
        ar->setRoomId(ui->roomDetailsTable->selectedItems().at(0)->data(Qt::UserRole).toInt());
        ar->setRoomParent(this);
        ar->setEditMode(true);
        ar->show();
    }
    else
    {
        QMessageBox msgBox;
        msgBox.critical(this,"Error","Please select a room.");
    }
}

void Rooms::on_deleteRoom_clicked()
{
    //int row = ui->roomDetailsTable->selectedItems().at(0)->data(Qt::UserRole).toInt();


    QMessageBox::StandardButton reply;
    QMessageBox msgBox;
    reply = QMessageBox::question(this, "Delete Rooms", "Are you want to delete this room?",QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::Yes) {

        DbMysql* d = DbMysql::getInstance();
        if (!d->getConnection().open()) {
            msgBox.critical(this,"Error","Failed to connect database.");
        }
        else {
            QSqlQuery query("delete from rooms where room_id="+QString::number(ui->roomDetailsTable->selectedItems().at(0)->data(Qt::UserRole).toInt())+";",d->getConnection());
            if( !query.isActive() )
            {
                msgBox.critical(this,"Error","Failed to execute query.");
                qDebug()<<query.lastError().text();
                return;
            }
            else{
                msgBox.information(this,"Information","Room Deleted.");
                this->getAllRoomsData();
            }
        }
    }
}
