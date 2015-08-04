#include "hotelinfo.h"
#include "ui_hotelinfo.h"
#include "dbmysql.h"
#include <QMessageBox>
#include <QDebug>
#include <QCryptographicHash>

HotelInfo::HotelInfo(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::HotelInfo)
{
    ui->setupUi(this);
    addHotelInfoInForm();
}

HotelInfo::~HotelInfo()
{
    delete ui;
}

void HotelInfo::on_cancelBtn_clicked()
{
    this->close();
}

void HotelInfo::addHotelInfoInForm()
{
    DbMysql* d = DbMysql::getInstance();
    QMessageBox msgBox;

    if(!d->getConnection().open())
    {
        msgBox.critical(this,"Error","Failed to connect database.1");
    }
    else
    {
        QString sql = "select * from hotel_info where hotel_info_id=1;";

        QSqlQuery query1(sql,d->getConnection());
        qDebug()<<query1.lastQuery();
        if(!query1.isActive())
        {
            msgBox.critical(this,"Error","Failed to connect database.");
        }
        else
        {
            query1.next();
            ui->hotelName->setText(query1.value(1).toString());
            ui->address->setPlainText(query1.value(2).toString());
            ui->city->setText(query1.value(3).toString());
            ui->dist->setText(query1.value(4).toString());
            ui->state->setText(query1.value(5).toString());
            ui->pin->setText(query1.value(6).toString());
            ui->phone1->setText(query1.value(7).toString());
            ui->phone2->setText(query1.value(8).toString());
            ui->email->setText(query1.value(9).toString());
            ui->website->setText(query1.value(10).toString());
            ui->cperson->setText(query1.value(11).toString());
            ui->taxId->setText(query1.value(12).toString());

        }
    }
}

void HotelInfo::on_saveInfoBtn_clicked()
{
    DbMysql* d = DbMysql::getInstance();
    QMessageBox msgBox;

    if(!d->getConnection().open())
    {
        msgBox.critical(this,"Error","Failed to connect database.1");
    }
    else
    {
        QString sql = "update hotel_info set hotel_name ='"+ui->hotelName->text();
        sql +="',address='"+ui->address->toPlainText();
        sql +="',city='"+ui->city->text();
        sql +="',dist='"+ui->dist->text();
        sql +="',state='"+ui->state->text();
        sql +="',pin='"+ui->pin->text();
        sql +="',phone1='"+ui->phone1->text();
        sql +="',phone2='"+ui->phone2->text();
        sql +="',email='"+ui->email->text();
        sql +="',website='"+ui->website->text();
        sql +="',contact_person='"+ui->cperson->text();
        sql +="',tax_id='"+ui->taxId->text()+"' where hotel_info_id=1;";

        QSqlQuery query1(sql,d->getConnection());
        qDebug()<<query1.lastQuery();
        if(!query1.isActive())
        {
            msgBox.critical(this,"Error","Failed to connect database.");
        }
        else
        {
            msgBox.information(this,"Save Hotel Information","Hotel Information Updated");
            this->close();
        }
    }
}
