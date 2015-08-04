#include "roomsettings.h"
#include "ui_roomsettings.h"
#include "dbmysql.h"
#include <QMessageBox>

RoomSettings::RoomSettings(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RoomSettings)
{
    ui->setupUi(this);
    addAllRoomTypes();
    addAllRoomEquipments();
}

RoomSettings::~RoomSettings()
{
    delete ui;
}

void RoomSettings::on_closeBtn_clicked()
{
    this->close();
}


void RoomSettings::addAllRoomTypes()
{

    DbMysql* d = DbMysql::getInstance();
    QMessageBox msgBox;
    if(!d->getConnection().open()){
        msgBox.critical(this,"Error","Failed to connect database.");
    }
    else {
        QSqlQuery query( "select room_type_id,room_type_name from room_types;" ,d->getConnection());
        if( !query.isActive() )
        {
            qDebug()<<"Failed to execute query. insert room.";
            qDebug()<<query.lastQuery();
            qDebug()<<query.lastError().text();
            return;
        }
        else
        {

            ui->roomTypeList->clear();
            int index=0;
            while(query.next())
            {
                QListWidgetItem* item = new QListWidgetItem();
                item->setText(query.value(1).toString().trimmed());
                //qDebug()<<item->text();

                item->setData(Qt::UserRole,QVariant::fromValue(query.value(0).toInt()));
                ui->roomTypeList->insertItem(index++,item);
            }
        }
    }
}


void RoomSettings::addAllRoomEquipments()
{

    DbMysql* d = DbMysql::getInstance();
    QMessageBox msgBox;
    if(!d->getConnection().open()){
        msgBox.critical(this,"Error","Failed to connect database.");
    }
    else {
        QSqlQuery query( "select * from room_equipments;" ,d->getConnection());
        if( !query.isActive() )
        {
            qDebug()<<"Failed to execute query. insert room.";
            qDebug()<<query.lastQuery();
            qDebug()<<query.lastError().text();
            return;
        }
        else
        {
            ui->equipList->clear();
            int index=0;
            while(query.next())
            {
                QListWidgetItem* item = new QListWidgetItem();
                item->setText(query.value(1).toString());

                qDebug()<<query.value(1).toString();

                item->setData(Qt::UserRole,QVariant::fromValue(query.value(0).toInt()));
                ui->equipList->insertItem(index++,item);
            }
        }
    }
}




void RoomSettings::on_addRoomType_clicked()
{
    DbMysql* d = DbMysql::getInstance();
    QMessageBox msgBox;
    if(!d->getConnection().open()){
        msgBox.critical(this,"Error","Failed to connect database.");
    }
    else {
        QSqlQuery query( "insert into room_types values(DEFAULT,'"+ui->roomType->text()+"');" ,d->getConnection());
        if( !query.isActive() )
        {
            qDebug()<<"Failed to execute query. insert room.";
            qDebug()<<query.lastQuery();
            qDebug()<<query.lastError().text();
            return;
        }
        else
        {
            msgBox.information(this,"Success","New room type added.");
            this->addAllRoomTypes();
            ui->roomType->setText("");
        }
    }
}

void RoomSettings::on_addEquip_clicked()
{
    DbMysql* d = DbMysql::getInstance();
    QMessageBox msgBox;
    if(!d->getConnection().open()){
        msgBox.critical(this,"Error","Failed to connect database.");
    }
    else {
        QSqlQuery query( "insert into room_equipments values(DEFAULT,'"+ui->equipName->text()+"');" ,d->getConnection());
        if( !query.isActive() )
        {
            qDebug()<<"Failed to execute query. insert room.";
            qDebug()<<query.lastQuery();
            qDebug()<<query.lastError().text();
            return;
        }
        else
        {
            msgBox.information(this,"Success","New equipment added.");
            this->addAllRoomEquipments();
            ui->equipName->setText("");
        }
    }
}

void RoomSettings::on_deleteRoom_clicked()
{
    DbMysql* d = DbMysql::getInstance();
    QMessageBox msgBox;
    if(!d->getConnection().open()){
        msgBox.critical(this,"Error","Failed to connect database.");
    }
    else {
        //qDebug()<<ui->roomTypeList->currentItem()->data(Qt::UserRole).toInt();
        QSqlQuery query( "delete from room_types where room_type_id='"+ui->roomTypeList->currentItem()->data(Qt::UserRole).toString()+"';" ,d->getConnection());
        if( !query.isActive() )
        {
            qDebug()<<"Failed to execute query. insert room.";
            qDebug()<<query.lastQuery();
            qDebug()<<query.lastError().text();
            return;
        }
        else
        {
            msgBox.information(this,"Success","Room type deleted.");
            this->addAllRoomTypes();
        }
    }
}

void RoomSettings::on_deleteEquip_clicked()
{
    DbMysql* d = DbMysql::getInstance();
    QMessageBox msgBox;
    if(!d->getConnection().open()){
        msgBox.critical(this,"Error","Failed to connect database.");
    }
    else {
        QSqlQuery query( "delete from room_equipments where equip_id='"+ui->equipList->currentItem()->data(Qt::UserRole).toString()+"';" ,d->getConnection());
        if( !query.isActive() )
        {
            qDebug()<<"Failed to execute query. insert room.";
            qDebug()<<query.lastQuery();
            qDebug()<<query.lastError().text();
            return;
        }
        else
        {
            msgBox.information(this,"Success","Room type deleted.");
            this->addAllRoomEquipments();
        }
    }
}
