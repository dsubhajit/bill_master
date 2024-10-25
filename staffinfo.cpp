#include "staffinfo.h"
#include "ui_staffinfo.h"
#include "addstaff.h"
#include "addstaffpayment.h"
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


StaffInfo::StaffInfo(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::StaffInfo)
{

    ui->setupUi(this);
    ui->staffName->setVisible(false);
    ui->staffNameLabel->setVisible(false);
    this->loadStaff();
    QStringList months;
    months<<"All"<<"Jan"<<"Feb"<<"Mar"<<"Apr"<<"May"<<"Jun"<<"Jul"<<"Aug"<<"Sep"<<"Oct"<<"Nov"<<"Dec";
    int i=0;
    int cur_mnth = QDate::currentDate().month();

    foreach (QString mnth, months) {
        ui->monthList->addItem(mnth,QVariant::fromValue(i++));

    }
    ui->monthList->setCurrentIndex(cur_mnth);

    int cur_year= QDate::currentDate().year();

    ui->yearList->addItem("All",QVariant::fromValue(-1));
    for(int i=2000;i<2050;i++)
    {
        ui->yearList->addItem(QString::number(i),QVariant::fromValue(i));
    }
    ui->yearList->setCurrentIndex(cur_year-2000+1);
    this->calculatePayment(1);
}

StaffInfo::~StaffInfo()
{
    delete ui;
}

void StaffInfo::on_addStaff_clicked()
{
    AddStaff* astf = new AddStaff();
    astf->setStaffInfoParent(this);
    astf->setAttribute(Qt::WA_DeleteOnClose);
    astf->show();
}

void StaffInfo::calculatePayment(int arType)
{
    DbMysql* d = DbMysql::getInstance();
    QMessageBox msgBox;
    int month = ui->monthList->currentData(Qt::UserRole).toInt();
    int year = ui->yearList->currentData(Qt::UserRole).toInt();
    if( month > 0)
    {
        ui->monthName->setText(QDate::fromString("2000-"+QString::number(month)+"-12","yyyy-M-d").toString("MMMM"));
    }
    else
    {
        ui->monthName->setText("All");
    }

    if(year > 0)
    {
        ui->yearNo->setText(QString::number(year));
    }
    else
    {
        ui->yearNo->setText("All");
    }

    if(arType == 1)
    {
        ui->staffName->setVisible(false);
        ui->staffNameLabel->setVisible(false);
        if(!d->getConnection().open()){
            msgBox.critical(this,"Error","Failed to connect database.");
        }
        else {
            QString sql="";
            bool nextFlg = false;
            if(month > 0)
            {
                sql +=" MONTH(payment_date) = "+QString::number(month);
                nextFlg = true;
            }

            if(year > 0)
            {
                if(nextFlg) sql+=" and ";
                sql +=" YEAR(payment_date) = "+QString::number(year);
            }

            if(sql.trimmed().length() == 0) sql = " 1 ";

            QSqlQuery query( "select sum(payment_amount) from staff_payment where "+sql+";" ,d->getConnection());
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
                ui->totalPayment->setText(query.value(0).toString()+".00");
            }
        }
    }

    if(arType == 2)
    {
        if(!d->getConnection().open()){
            msgBox.critical(this,"Error","Failed to connect database.");
        }
        else {
            QString sql="";

            if(month > 0)
            {
                sql +=" MONTH(payment_date) = "+QString::number(month)+" and ";
            }

            if(year > 0)
            {
                sql +=" YEAR(payment_date) = "+QString::number(year)+" and ";
            }


            QSqlQuery query( "select sum(payment_amount) from staff_payment where "+sql+" staff_id = "+QString::number(ui->staffListTree->selectedItems().at(0)->data(0,Qt::UserRole).toInt())+";" ,d->getConnection());
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
                ui->totalPayment->setText(query.value(0).toString()+".00");
            }
        }
    }
}

void StaffInfo::loadStaff(){
    DbMysql* d = DbMysql::getInstance();
    QMessageBox msgBox;
    if(!d->getConnection().open()){
        msgBox.critical(this,"Error","Failed to connect database.");
    }
    else {
        QSqlQuery query( "select * from staff_details;" ,d->getConnection());
        if( !query.isActive() )
        {
            qDebug()<<"Failed to execute query. Add Staff.";
            qDebug()<<query.lastQuery();
            qDebug()<<query.lastError().text();
            return;
        }
        else
        {
            ui->staffListTree->clear();
             while(query.next()){
                QTreeWidgetItem *item = new QTreeWidgetItem;
                item->setText(0,query.value("staff_name").toString());
                item->setData(0,Qt::UserRole,QVariant::fromValue(query.value("staff_id").toInt()));
                ui->staffListTree->addTopLevelItem(item);
             }
        }
    }
}

void StaffInfo::on_deleteStaff_clicked()
{
    QMessageBox msgBox;
    if(ui->staffListTree->selectedItems().size() > 0)
    {
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, "Delete Staff", "Are you sure?",
                                    QMessageBox::Yes|QMessageBox::No);
        if (reply == QMessageBox::Yes)
        {
            DbMysql* d = DbMysql::getInstance();
            if(!d->getConnection().open()){
                msgBox.critical(this,"Error","Failed to connect database.");
            }
            else {
                QSqlQuery query( "delete from staff_details where staff_id="+QString::number(ui->staffListTree->selectedItems().at(0)->data(0,Qt::UserRole).toInt())+";" ,d->getConnection());
                if( !query.isActive() )
                {
                    qDebug()<<"Failed to execute query. Add Staff.";
                    qDebug()<<query.lastQuery();
                    qDebug()<<query.lastError().text();
                    return;
                }
                else
                {
                    this->loadStaff();
                    msgBox.information(this,"Delete Staff Details","Staff Deletd Successfully");
                }
            }
        }
    }
    else msgBox.warning(this,"Delete Staff Details","Please select a staff from the list");
}

void StaffInfo::on_editStaffDetails_clicked()
{
    QMessageBox msgBox;
    if(ui->staffListTree->selectedItems().size() > 0)
    {
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, "Edit Staff", "Are you sure?",
                                    QMessageBox::Yes|QMessageBox::No);
        if (reply == QMessageBox::Yes)
        {
            AddStaff* astf = new AddStaff;
            astf->setAttribute(Qt::WA_DeleteOnClose);
            astf->setEditMode(true);
            astf->setStaffInfoParent(this);
            astf->loadStaffDataForId(ui->staffListTree->selectedItems().at(0)->data(0,Qt::UserRole).toInt());
            astf->show();
        }
    }
    else msgBox.warning(this,"Delete Staff Details","Please select a staff from the list");
}

void StaffInfo::on_addPayment_clicked()
{
    QMessageBox msgBox;
    if(ui->staffListTree->selectedItems().size() > 0)
    {
        AddStaffPayment *sp = new AddStaffPayment;
        sp->setAttribute(Qt::WA_DeleteOnClose);
        sp->setStafInfoParent(this);
        sp->setStaffId(ui->staffListTree->selectedItems().at(0)->data(0,Qt::UserRole).toInt());
        sp->show();
    }
    else msgBox.warning(this,"Add Staff Payment","Please select a staff from the list");
}

void StaffInfo::on_staffListTree_itemDoubleClicked(QTreeWidgetItem *item, int column)
{
    QMessageBox msgBox;
    DbMysql* d = DbMysql::getInstance();
    if(!d->getConnection().open()){
        msgBox.critical(this,"Error","Failed to connect database.");
    }
    else
    {
        int month = ui->monthList->currentData(Qt::UserRole).toInt();
        int year = ui->yearList->currentData(Qt::UserRole).toInt();

        QString sql="";

        if(month > 0)
        {
            sql +=" MONTH(payment_date) = "+QString::number(month)+" and ";
        }

        if(year > 0)
        {
            sql +=" YEAR(payment_date) = "+QString::number(year)+" and ";
        }


        QSqlQuery query( "select * from staff_payment where "+sql+" staff_id = "+QString::number(item->data(0,Qt::UserRole).toInt())+";" ,d->getConnection());
        if( !query.isActive() )
        {
            qDebug()<<"Failed to execute query. Add Staff.";
            qDebug()<<query.lastQuery();
            qDebug()<<query.lastError().text();
            return;
        }
        else
        {
            qDebug()<<query.lastQuery();
            ui->paymentDetailsTable->setRowCount(query.size());
            int i = 0;
            double total = 0;
            while(query.next())
            {
                QTableWidgetItem *item = new QTableWidgetItem;
                item->setText(query.value("payment_date").toString());
                item->setData(Qt::UserRole,QVariant::fromValue(query.value("payment_id").toInt()));
                ui->paymentDetailsTable->setItem(i,0,item);

                QTableWidgetItem *item1 = new QTableWidgetItem;
                item1->setText(query.value("entry_date").toString());
                item1->setData(Qt::UserRole,QVariant::fromValue(query.value("payment_id").toInt()));
                ui->paymentDetailsTable->setItem(i,1,item1);

                QTableWidgetItem *item2 = new QTableWidgetItem;
                item2->setText(query.value("payment_amount").toString()+".00");
                item2->setData(Qt::UserRole,QVariant::fromValue(query.value("payment_id").toInt()));
                ui->paymentDetailsTable->setItem(i,2,item2);

                total+= query.value("payment_amount").toDouble();

                QTableWidgetItem *item3 = new QTableWidgetItem;
                item3->setText(query.value("remarks").toString());
                item3->setData(Qt::UserRole,QVariant::fromValue(query.value("payment_id").toInt()));
                ui->paymentDetailsTable->setItem(i,3,item3);

                i++;
            }
        }
    }

    ui->staffName->setVisible(true);
    ui->staffNameLabel->setVisible(true);
    ui->staffName->setText(ui->staffListTree->selectedItems().at(0)->text(0));
    this->calculatePayment(2);
}

void StaffInfo::reloadPayments()
{
    QMessageBox msgBox;
    if(ui->staffListTree->selectedItems().size() > 0)
    {
        this->on_staffListTree_itemDoubleClicked(ui->staffListTree->selectedItems().at(0),0);
    }
    else
    {
        msgBox.warning(this,"Staff Details/Payments","Please select a staff from the list");
    }
}

void StaffInfo::on_paymentFilter_clicked()
{
    QMessageBox msgBox;
    if(ui->staffListTree->selectedItems().size() > 0)
    {
        this->on_staffListTree_itemDoubleClicked(ui->staffListTree->selectedItems().at(0),0);
        ui->staffName->setVisible(true);
        ui->staffNameLabel->setVisible(true);
        ui->staffName->setText(ui->staffListTree->selectedItems().at(0)->text(0));
        this->calculatePayment(2);
    }
    else
    {
        msgBox.warning(this,"Staff Details/Payments","Please select a staff from the list");
    }
}

void StaffInfo::on_pushButton_2_clicked()
{
    QMessageBox msgBox;
    if(ui->paymentDetailsTable->selectedItems().size() > 0)
    {
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, "Delete Payments", "Are you sure?",
                                    QMessageBox::Yes|QMessageBox::No);
        if (reply == QMessageBox::Yes)
        {
            DbMysql* d = DbMysql::getInstance();
            if(!d->getConnection().open()){
                msgBox.critical(this,"Error","Failed to connect database.");
            }
            else {
                QSqlQuery query( "delete from staff_payment where payment_id="+QString::number(ui->paymentDetailsTable->selectedItems().at(0)->data(Qt::UserRole).toInt())+";" ,d->getConnection());
                if( !query.isActive() )
                {
                    qDebug()<<"Failed to execute query. Add Staff.";
                    qDebug()<<query.lastQuery();
                    qDebug()<<query.lastError().text();
                    return;
                }
                else
                {
                    if(ui->staffListTree->selectedItems().size() > 0)
                    {
                        this->on_staffListTree_itemDoubleClicked(ui->staffListTree->selectedItems().at(0),0);
                    }
                    msgBox.information(this,"Delete Staff Payment","Staff Payment Deletd Successfully");
                }
            }
        }
    }
    else msgBox.warning(this,"Delete Staff Payment","Please select a payment from the below table");
}

void StaffInfo::on_showTotalStaffPayment_clicked()
{
    this->calculatePayment(1);
}
