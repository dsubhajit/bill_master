#include "addstaffpayment.h"
#include "ui_addstaffpayment.h"
#include "dbmysql.h"
#include <QMessageBox>
#include <QtPrintSupport/QPrinter>
#include <QTextDocument>
#include <QtPrintSupport/QPrintDialog>
#include <QMessageBox>
#include <QPainter>
//#include <QWebView>
#include <QPrintDialog>
#include <QTextCursor>
#include <QMargins>
#include <QTextBlockFormat>
#include <QDebug>
#include <QScrollBar>
#include <QTreeWidgetItem>



AddStaffPayment::AddStaffPayment(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddStaffPayment)
{
    ui->setupUi(this);
    ui->payment_date->setDate(QDate::currentDate());
}

AddStaffPayment::~AddStaffPayment()
{
    delete ui;
}

void AddStaffPayment::setStafInfoParent(StaffInfo *arP)
{
    this->mStaffInfoParent = arP;
}

void AddStaffPayment::setStaffId(int arId)
{
    this->staff_id = arId;
    DbMysql* d = DbMysql::getInstance();
    QMessageBox msgBox;
    if(!d->getConnection().open()){
        msgBox.critical(this,"Error","Failed to connect database.");
    }
    else {
        QSqlQuery query( "select * from staff_details where staff_id="+QString::number(arId) ,d->getConnection());
        if( !query.isActive() )
        {
            qDebug()<<"Failed to execute query. Add Staff.";
            qDebug()<<query.lastQuery();
            qDebug()<<query.lastError().text();
            return;
        }
        else
        {
             query.next();
             ui->staff_name->setText(query.value("staff_name").toString());
             ui->staff_phone->setText(query.value("staff_phone").toString());

        }
    }
}

void AddStaffPayment::on_cancelPayment_clicked()
{
    this->close();
}

void AddStaffPayment::on_savePayment_clicked()
{
    DbMysql* d = DbMysql::getInstance();
    QMessageBox msgBox;
    if(!d->getConnection().open()){
        msgBox.critical(this,"Error","Failed to connect database.");
    }
    else {
        QSqlQuery query( "insert into staff_payment values (default,"+QString::number(this->staff_id)+",'"+ui->payment_amount->text()+"','"+ui->payment_date->date().toString("yyyy-MM-dd")+"',NOW(),'"+ui->payment_remarks->toPlainText()+"')" ,d->getConnection());
        if( !query.isActive() )
        {
            qDebug()<<"Failed to execute query. Add Staff.";
            qDebug()<<query.lastQuery();
            qDebug()<<query.lastError().text();
            return;
        }
        else
        {
            msgBox.information(this,"Staff Payment ","Staff payment entry successfull");
            this->mStaffInfoParent->reloadPayments();
            this->close();
        }
    }
}


