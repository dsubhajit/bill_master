#include "logbook.h"
#include "ui_logbook.h"
#include "dbmysql.h"
#include "createlog.h"
#include "createinvoices.h"
#include <QMessageBox>

LogBook::LogBook(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LogBook)
{
    ui->setupUi(this);
    ui->fromDate->setDate(QDate::currentDate().addDays(-5));
    ui->toDate->setDate(QDate::currentDate().addDays(1));
    this->populateLogBookTable();
}

LogBook::~LogBook()
{
    delete ui;
}

void LogBook::populateLogBookTable()
{
    DbMysql* d = DbMysql::getInstance();
    QMessageBox msgBox;

    if(!d->getConnection().open())
    {
        msgBox.critical(this,"Error","Failed to connect database.1");
    }
    else
    {
        QString from_range_date = ui->fromDate->date().toString("yyyy-MM-dd");
        QString to_range_date = ui->toDate->date().toString("yyyy-MM-dd");
        QSqlQuery query1( "SELECT booking_log.log_id,details.customer_name,details.room_number,details.booking_from,details.booking_to,booking_log.check_in_time,booking_log.check_out_time,booking_log.reg_serial,details.id_type,details.id_serial,details.nop,details.ps_type,details.booking_id FROM booking_log left join ( select booking.booking_id,customers.customer_name,rooms.room_number,customers.id_type,customers.id_serial,booking.booking_from,booking.booking_to,booking.nop,payment_status.ps_type from booking left join rooms on booking.room_id = rooms.room_id left join customers on customers.customer_id = booking.customer_id left join payment_status on payment_status.ps_id = booking.payment_status) as details on booking_log.booking_id = details.booking_id where check_in_time>='"+from_range_date+"' and check_in_time<='"+to_range_date+"' order by log_id desc",d->getConnection());
        qDebug()<<query1.lastQuery();
        if(!query1.isActive())
        {
            msgBox.critical(this,"Error","Failed to connect database.");
        }
        else
        {
            ui->logbook->setRowCount(query1.size());
            int row = 0 ;

            while(query1.next())
            {

                int columnSize = ui->logbook->columnCount();
                qDebug()<<"Column Size:"<<columnSize;

                for( int column = 0; column < columnSize; column++)
                {
                    QTableWidgetItem *newItem = new QTableWidgetItem();
                    newItem->setData(Qt::UserRole,QVariant::fromValue(query1.value(0).toInt()));

                    //newItem->setData(Qt::,QVariant::fromValue(query1.value(columnSize).toInt()));
                    newItem->setText(query1.value(column).toString());
                    ui->logbook->setItem(row, column, newItem);

                }

                row++;
            }
            ui->logbook->resizeColumnsToContents();
            ui->logbook->horizontalHeader()->setHighlightSections(false);
            ui->logbook->horizontalHeader()->setStyleSheet("QHeaderView::section { background-color:#0B2161;color:#fff; }");

        }
    }


}

void LogBook::on_logbook_cellDoubleClicked(int row, int column)
{

    int log_id =  ui->logbook->item(row,0)->data(Qt::UserRole).toInt();
    int booking_id;
    DbMysql* d = DbMysql::getInstance();
    QMessageBox msgBox;

    column = 0;
    //ui->check_in_time->setDateTime(QDateTime::currentDateTime());

    if(!d->getConnection().open())
    {
        msgBox.critical(this,"Error","Failed to connect database.1");
    }
    else
    {
        QSqlQuery query( "select booking_id from booking_log where log_id="+QString::number(log_id)+";",d->getConnection());

        if(query.isActive())
        {
            while(query.next())
            {
                booking_id = query.value(0).toInt();
            }
        }
        else return;
    }

    CreateLog* cl = new CreateLog();
    cl->setEditMode(true,log_id);
    cl->setLogBookObject(this);
    cl->setDataFields(booking_id);

    cl->show();
}



void LogBook::on_createInvoice_clicked()
{
    QMessageBox msgBox;
    if( ui->logbook->selectedItems().size() > 0)
    {
        int logId = ui->logbook->selectedItems().at(0)->data(Qt::UserRole).toInt();
        CreateInvoices* cInv =  new CreateInvoices();
        cInv->addBookingData(logId);
        cInv->show();
    }
    else
    {
        msgBox.warning(this,"Create Invoice","Please select a log book entry to create invoice.");
        return;
    }
}
