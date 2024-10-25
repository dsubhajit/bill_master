#include "addrooms.h"
#include "ui_addrooms.h"
#include "ui_rooms.h"
#include "dbmysql.h"
#include <QMessageBox>
#include "ui_mainwindow.h"
#include "rooms.h"
#include "mainwindow.h"


AddRooms::AddRooms(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AddRooms)
{
    ui->setupUi(this);
    this->mEditMode = false;
    addAllRoomEquipments();
    addAllRoomTypes();
}

AddRooms::~AddRooms()
{
    delete ui;
}

void AddRooms::addAllRoomTypes()
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
            while(query.next())
            {

                ui->roomType->addItem(query.value(1).toString());
            }
        }
    }
}


void AddRooms::addAllRoomEquipments()
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
            ui->equipments->clear();
            while(query.next())
            {
                QListWidgetItem* item = new QListWidgetItem();
                item->setText(query.value(1).toString());
                qDebug()<<query.value(1).toString();

                item->setData(Qt::UserRole,QVariant::fromValue(query.value(1).toString()));
                ui->equipments->addItem(item);
            }
        }
    }
}


void AddRooms::setEditMode(bool eMode)
{
    this->mEditMode = eMode;
    if(this->mEditMode)
    {
        this->setWindowTitle("Edit room details");
        ui->saveRoom->setText("Update");
        DbMysql* d = DbMysql::getInstance();
        QMessageBox msgBox;
        if(!d->getConnection().open()){
            msgBox.critical(this,"Error","Failed to connect database.");
        }
        else {

            QSqlQuery query1( "select * from rooms where room_id='"+QString::number(mRoomId)+"';" ,d->getConnection());

            if(!query1.isActive())
            {
                msgBox.critical(this,"Error","Failed to connect database.");
                return;
            }
            else
            {
                while(query1.next())
                {
                    ui->roomNumber->setText(query1.value("room_number").toString());
                    qDebug()<<query1.lastQuery();
                    qDebug()<<query1.value("room_number").toString();
                    ui->roomType->setCurrentText(query1.value("room_type").toString());
                    ui->roomDescription->setText(query1.value("room_desc").toString());
                    ui->singleBeds->setValue(query1.value("single_beds").toInt());
                    ui->doubleBeds->setValue(query1.value("double_beds").toInt());
                    ui->extraBed->setValue(query1.value("extra_beds").toInt());


                    QStringList equip = query1.value("room_equip").toString().split(',');
                    /*
                    foreach (QString eq,equip)
                    {
                        QListWidgetItem* item = new QListWidgetItem();
                        item->setText(eq);
                        ui->equipments->setItemSelected(item,true);

                    }
                    */

                    for(int i = 0; i < ui->equipments->count(); ++i)
                    {
                        QListWidgetItem* item = ui->equipments->item(i);
                        if(equip.contains(item->text()))
                        {
                            item->setSelected(true);
                        }
                    }






                    ui->rate_per_room->setText(query1.value("rate_per_room").toString());
                    ui->rate_per_person->setText(query1.value("rate_per_person").toString());
                    int room_status = query1.value("room_status").toInt();
                    ui->cGst->setValue(query1.value("cgst").toDouble());
                    ui->sGst->setValue(query1.value("sgst").toDouble());
                    ui->gst->setValue(query1.value("gst").toDouble());

                    switch(room_status)
                    {
                        case 1: ui->room_status_clean_checked->setChecked(true);break;
                        case 2: ui->room_status_clean->setChecked(true);break;
                        case 3: ui->room_status_need_cleaning->setChecked(true);break;
                        default: ui->room_status_none->setChecked(true);break;
                    }
                }
            }
        }
    }
}

void AddRooms::setRoomId(int roomId)
{
    this->mRoomId = roomId;
}

void AddRooms::on_cancelAddNewRoom_clicked()
{
    this->close();
}

void AddRooms::setRoomParent(Rooms *arm)
{
    this->mRooms = arm;
}

void AddRooms::on_saveRoom_clicked()
{
    DbMysql* d = DbMysql::getInstance();
    QMessageBox msgBox;
    if(!d->getConnection().open()){
        msgBox.critical(this,"Error","Failed to connect database.");
    }
    else {
        if(!mEditMode)
        {
            QSqlQuery query1( "select * from rooms where room_number='"+ui->roomNumber->text()+"';" ,d->getConnection());
            qDebug()<<query1.lastError().text();
            qDebug()<<query1.lastQuery();
            if(query1.size()>0)
            {
                msgBox.critical(this,"Error","Room number "+ui->roomNumber->text()+" already exists. Please try with another one.");
                return;
            }
        }

        int room_status = 0;
        if(ui->room_status_clean->isChecked()){
            room_status = 2;
        }
        else if(ui->room_status_clean_checked->isChecked()) room_status = 1;
        else if(ui->room_status_need_cleaning->isChecked()) room_status = 3;
        else room_status = 0;

        double rate_per_room = ui->rate_per_room->text().toDouble();
        double rate_per_person = ui->rate_per_person->text().toDouble();

        QList<QListWidgetItem*> s = ui->equipments->selectedItems();

        QString equip;
        foreach (QListWidgetItem* item, s)
        {
            equip+=item->text()+",";
        }

        qDebug()<<equip;

        if(!mEditMode)
        {
            QSqlQuery query( "insert into rooms values(DEFAULT,'"+ui->roomNumber->text()+"','"+ui->roomType->currentText()+"','"+ui->roomDescription->toPlainText()+"',"+ui->singleBeds->text()+","+ui->doubleBeds->text()+","+ui->extraBed->text()+",'"+equip+"',"+QString::number(rate_per_room)+","+QString::number(rate_per_person)+","+QString::number(room_status)+",0,"+QString::number(ui->cGst->value())+","+QString::number(ui->sGst->value())+","+QString::number(ui->gst->value())+");" ,d->getConnection());
            if( !query.isActive() )
            {
                qDebug()<<"Failed to execute query. insert room.";
                qDebug()<<query.lastQuery();
                qDebug()<<query.lastError().text();
                return;
            }
            else
            {
                msgBox.information(this,"Success","New room added.");
                this->mRooms->getAllRoomsData();
                this->close();
            }
        }
        else
        {
            QSqlQuery query( "update rooms set room_number='"+ui->roomNumber->text()+"',room_equip='"+equip+"',room_type='"+ui->roomType->currentText()+"',room_desc='"+ui->roomDescription->toPlainText()+"',single_beds="+ui->singleBeds->text()+",double_beds="+ui->doubleBeds->text()+",extra_beds="+ui->extraBed->text()+",rate_per_room="+QString::number(rate_per_room)+",rate_per_person="+QString::number(rate_per_person)+",room_status="+QString::number(room_status)+" ,cgst="+QString::number(ui->cGst->value())+",sgst="+QString::number(ui->sGst->value())+",gst="+QString::number(ui->gst->value())+" where room_id="+QString::number(mRoomId)+";" ,d->getConnection());
            if( !query.isActive() )
            {
                qDebug()<<"Failed to execute query. insert room.";
                qDebug()<<query.lastQuery();
                qDebug()<<query.lastError().text();
                return;
            }
            else
            {
                msgBox.information(this,"Success","Room details updated.");
                this->mRooms->getAllRoomsData();
                this->close();
            }

        }
    }
}

void AddRooms::on_cGst_valueChanged(double arg1)
{
    ui->gst->setValue(arg1+ui->sGst->value());
}

void AddRooms::on_sGst_valueChanged(double arg1)
{
    ui->gst->setValue(arg1+ui->cGst->value());
}
