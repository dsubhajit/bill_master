#include "addadvancepayment.h"
#include "ui_addadvancepayment.h"
#include "dbmysql.h"
#include <QMessageBox>


AddAdvancePayment::AddAdvancePayment(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddAdvancePayment)
{
    ui->setupUi(this);
}

AddAdvancePayment::~AddAdvancePayment()
{
    delete ui;
}

void AddAdvancePayment::on_addPayment_clicked()
{
    DbMysql* d = DbMysql::getInstance();
    QMessageBox msgBox;
    if(!d->getConnection().open()){
        msgBox.critical(this,"Error","Failed to connect database.");
    }
    else {
        QSqlQuery query( "insert into advance_payments values(DEFAULT,"+ui->amount->text()+","+QString::number(mBookingId)+",NOW());" ,d->getConnection());
        if( !query.isActive() )
        {
            qDebug()<<"Failed to execute query. insert room.";
            qDebug()<<query.lastQuery();
            qDebug()<<query.lastError().text();
            return;
        }
        else
        {
            msgBox.information(this,"Success","Advance payment added.");
            getAllPayments();
        }
    }

}

void AddAdvancePayment::setBookingId(int bookingId)
{
    mBookingId = bookingId;
}

void AddAdvancePayment::getAllPayments()
{
    DbMysql* d = DbMysql::getInstance();
    QMessageBox msgBox;
    if(!d->getConnection().open()){
        msgBox.critical(this,"Error","Failed to connect database.");
    }
    else {
        QSqlQuery query( "select * from advance_payments where booking_id = "+QString::number(mBookingId)+";" ,d->getConnection());
        if( !query.isActive() )
        {
            qDebug()<<"Failed to execute query. insert room.";
            qDebug()<<query.lastQuery();
            qDebug()<<query.lastError().text();
            return;
        }
        else
        {
            int i = 0;
            double totalPayment = 0;
            ui->paymentTable->setRowCount(query.size());
            while(query.next())
            {
                QTableWidgetItem* item1 = new QTableWidgetItem();
                item1->setText(QString::number(i+1));
                item1->setData(Qt::UserRole,QVariant::fromValue(query.value("payment_id").toInt()));

                QTableWidgetItem* item2 = new QTableWidgetItem();
                item2->setText(query.value("payment_time").toString());
                item2->setData(Qt::UserRole,QVariant::fromValue(query.value("payment_id").toInt()));

                QTableWidgetItem* item3 = new QTableWidgetItem();
                item3->setText(QString::number(query.value("payment_value").toDouble(),'f',2));
                item3->setData(Qt::UserRole,QVariant::fromValue(query.value("payment_id").toInt()));

                ui->paymentTable->setItem(i,0,item1);
                ui->paymentTable->setItem(i,1,item2);
                ui->paymentTable->setItem(i,2,item3);
                totalPayment+= query.value("payment_value").toDouble();
                i++;
            }
            ui->totalAdvancePaid->setValue(totalPayment);
        }
    }
}

void AddAdvancePayment::on_deletePayment_clicked()
{
    QMessageBox msgBox;
    if(ui->paymentTable->selectedItems().size() > 0)
    {
        DbMysql* d = DbMysql::getInstance();

        if(!d->getConnection().open()){
            msgBox.critical(this,"Error","Failed to connect database.");
        }
        else {
            QSqlQuery query( "delete from advance_payments where payment_id="+QString::number(ui->paymentTable->selectedItems().at(0)->data(Qt::UserRole).toInt())+";" ,d->getConnection());
            if( !query.isActive() )
            {
                qDebug()<<"Failed to execute query. insert room.";
                qDebug()<<query.lastQuery();
                qDebug()<<query.lastError().text();
                return;
            }
            else
            {
                msgBox.information(this,"Success","Advance payment deleted.");
                getAllPayments();
            }
        }
    }
    else
    {
        msgBox.information(this,"Delete Advance Payment","Please select a booking from table.");
    }

}
