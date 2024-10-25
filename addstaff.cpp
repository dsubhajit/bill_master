#include "addstaff.h"
#include "ui_addstaff.h"
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

AddStaff::AddStaff(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddStaff)
{

    this->mEditMode = false;
    this->staffId = 0;
    ui->setupUi(this);
    ui->staff_join_date->setDate(QDate::currentDate());
}

void AddStaff::setStaffInfoParent(StaffInfo *arP){
    this->mStaffInfoPtr = arP;
}

AddStaff::~AddStaff()
{
    delete ui;
}

void AddStaff::setEditMode(bool arMode)
{
    this->mEditMode = arMode;
}

void AddStaff::loadStaffDataForId(int arId)
{
    this->staffId = arId;
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
             ui->staff_add->setPlainText(query.value("staff_add").toString());
             ui->staff_phone->setText(query.value("staff_phone").toString());
             ui->staff_remarks->setPlainText(query.value("staff_remarks").toString());
             ui->staff_join_date->setDate(QDate::fromString(query.value("staff_join").toString(),"yyyy-MM-dd"));
        }
    }
}

void AddStaff::on_cancelBtn_clicked()
{
    this->close();
}

void AddStaff::on_saveStaff_clicked()
{
    DbMysql* d = DbMysql::getInstance();
    QMessageBox msgBox;
    if(!d->getConnection().open()){
        msgBox.critical(this,"Error","Failed to connect database.");
    }
    else {

        if(this->mEditMode)
        {
            QSqlQuery query( "update staff_details set staff_name='"+ui->staff_name->text()+"',staff_add='"+ui->staff_add->toPlainText()+"',staff_phone='"+ui->staff_phone->text()+"',staff_join='"+ui->staff_join_date->date().toString("yyyy-MM-dd")+"',staff_remarks='"+ui->staff_remarks->toPlainText()+"' where staff_id ="+QString::number(this->staffId)+" ;" ,d->getConnection());
            if( !query.isActive() )
            {
                qDebug()<<"Failed to execute query. Add Staff.";
                qDebug()<<query.lastQuery();
                qDebug()<<query.lastError().text();
                return;
            }
            else
            {
                msgBox.information(this,"Edit Staff Detials","Staff details updated successfully.");

            }
        }
        else
        {
            QSqlQuery query( "insert into staff_details values (default,'"+ui->staff_name->text()+"','"+ui->staff_add->toPlainText()+"','"+ui->staff_phone->text()+"','"+ui->staff_join_date->date().toString("yyyy-MM-dd")+"','"+ui->staff_remarks->toPlainText()+"');" ,d->getConnection());
            if( !query.isActive() )
            {
                qDebug()<<"Failed to execute query. Add Staff.";
                qDebug()<<query.lastQuery();
                qDebug()<<query.lastError().text();
                return;
            }
            else
            {
                 msgBox.information(this,"Add Staff Detials","Staff details added successfully.");
            }

        }
        if(this->mStaffInfoPtr != NULL){
            this->mStaffInfoPtr->loadStaff();
        }
        this->close();

    }
}
