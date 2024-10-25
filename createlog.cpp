#include "createlog.h"
#include "ui_createlog.h"

#include "logbook.h"
#include "dbmysql.h"
#include <QDebug>
#include <QScrollBar>
#include <QMessageBox>
#include <QHash>
#include <QCompleter>
#include <QStandardItem>

CreateLog::CreateLog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CreateLog)
{
    ui->setupUi(this);
    ui->check_out_time_label->setHidden(false);
    ui->check_out_time->setHidden(false);
    mEditMode = false;
}

void CreateLog::setLogBookObject(LogBook* aObj )
{
    this->mLogBookObj = aObj;
}

CreateLog::~CreateLog()
{
    delete ui;

}

void CreateLog::setBookingCalObject(BookingCalendar *aObj)
{
    this->mBookingCal = aObj;
}

bool CreateLog::checkIfRoomNotVacant(QString room_ids)
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
        QSqlQuery query1( "select * from rooms where room_id in ("+room_ids+") and room_state=1;" ,d->getConnection());
        if(!query1.isActive())
        {
            msgBox.critical(this,"Error","Failed to connect database.");
        }
        else
        {
            if(query1.size() > 0)
            {
                return false;
            }
            else return true;
        }
    }
}

void CreateLog::setDataFields(int booking_id)
{
    this->mBookingId = booking_id;
    DbMysql* d = DbMysql::getInstance();
    QMessageBox msgBox;

    ui->check_in_time->setDateTime(QDateTime::currentDateTime());

    if(!d->getConnection().open())
    {
        msgBox.critical(this,"Error","Failed to connect database.1");
    }
    else
    {
        QSqlQuery query1( "select * from booking_details where booking_id="+QString::number(booking_id)+";" ,d->getConnection());
        if(!query1.isActive())
        {
            msgBox.critical(this,"Error","Failed to connect database.");
        }
        else
        {
            query1.next();
            ui->customer_name->setText(query1.value("customer_name").toString());
            ui->email->setText(query1.value("email").toString());
            ui->phone->setText(query1.value("phone").toString());
            ui->address->setText(query1.value("address").toString());
            ui->id_type->setText(query1.value("id_type").toString());
            ui->id_serial->setText(query1.value("id_serial").toString());

            ui->booking_no->setText(query1.value("booking_id").toString());

            QDate frm_dt = QDate::fromString(query1.value("booking_from").toString(),"yyyy-MM-dd");
            QDate to_dt = QDate::fromString(query1.value("booking_to").toString(),"yyyy-MM-dd");

            if(frm_dt.daysTo(to_dt) == 0)
            {
                ui->label_from_date->setText("Booking Date: ");
                ui->from_date->setText(query1.value("booking_from").toString());
                ui->label_to_date->setText("");
                //ui->to_date->setText(query1.value("booking_to").toString());
            }
            else
            {
                ui->from_date->setText(query1.value("booking_from").toString());
                ui->to_date->setText(query1.value("booking_to").toString());
            }
            ui->nop->setText(query1.value("nop").toString());
            ui->room_no->setText(query1.value("room_numbers").toString());

            ui->check_in_time->setDateTime(query1.value("in_time").toDateTime());
            ui->check_out_time->setDateTime(query1.value("out_time").toDateTime());

           // qDebug()<<" CHeck Out:"<<query1.value("out_time").toString();
            if(mEditMode)
            {
                QSqlQuery query4( "select * from booking_log where log_id="+QString::number(mLogId)+";" ,d->getConnection());
                qDebug()<<query4.lastQuery();
                if(!query4.isActive())
                {
                    msgBox.critical(this,"Error","Failed to connect database.");
                }
                else
                {
                    while(query4.next())
                    {
                        //qDebug()<<;
                        ui->check_in_time->setDateTime(query4.value("check_in_time").toDateTime());
                        ui->regSerial->setText(query4.value("reg_serial").toString());
                    }
                }
            }
            else
            {
                qDebug()<<"WTF!!";
            }
        }
    }
}

void CreateLog::on_cancelBtn_clicked()
{
    this->close();
}

void CreateLog::on_saveBtn_clicked()
{
    int booking_id = this->mBookingId;
    QDateTime check_in_time = ui->check_in_time->dateTime();
    QDateTime check_out_time = ui->check_out_time->dateTime();
    QString regSerial = ui->regSerial->text();

    DbMysql* d = DbMysql::getInstance();
    QMessageBox msgBox;
/*
    if(!d->getConnection().open())
    {
        msgBox.critical(this,"Error","Failed to connect database.1");
        return;
    }
    else
    {
        QSqlQuery query1( "select * from room_booking_ids where booking_id="+QString::number(mBookingId)+";" ,d->getConnection());
        if(!query1.isActive())
        {
            msgBox.critical(this,"Error","Failed to connect database.");
            return;
        }
        else
        {
            query1.next();
            if(!checkIfRoomNotVacant(query1.value(1).toString()))
            {
                msgBox.warning(this,"Check In Customer","Some rooms are not vacant. Please check out those rooms.");
                return;
            }
        }
    }
*/


    ui->check_in_time->setDateTime(QDateTime::currentDateTime());

    if(!d->getConnection().open())
    {
        msgBox.critical(this,"Error","Failed to connect database.1");
    }
    else
    {

        QSqlQuery query1( "update booking set in_time='"+check_in_time.toString("yyyy-MM-dd HH:mm:ss")+"',out_time='"+check_out_time.toString("yyyy-MM-dd HH:mm:ss")+"',reg_serial='"+regSerial+"' where booking_id="+QString::number(booking_id)+";" ,d->getConnection());

        if(!query1.isActive())
        {
            qDebug()<<query1.lastQuery();
            msgBox.critical(this,"Error","Failed to connect database.");
        }
        else
        {
            QSqlQuery query("SELECT GROUP_CONCAT(room_id) from room_bookings WHERE booking_id ="+QString::number(booking_id)+" group by booking_id");
            if(query1.isActive())
            {
                query.next();
                QSqlQuery query2("update rooms set room_state=1 where room_id in ("+query.value(0).toString()+"); ",d->getConnection());
                if(query2.isActive())
                {
                    msgBox.information(this,"Customer Check In","Customer Check In Done.");
                    if(this->mBookingCal != NULL)
                    {
                        this->mBookingCal->populateBookingLog();
                    }
                }
                else qDebug()<<query2.lastQuery();
            }
            else qDebug()<<query.lastQuery();
        }
    }
    this->close();
}


void CreateLog::setEditMode(bool editmode, int logId)
{
    mEditMode = editmode;
    mLogId = logId;
    if(mEditMode)
    {
        ui->check_out_time_label->setHidden(false);
        ui->check_out_time->setHidden(false);
        ui->check_out_time->setDateTime(QDateTime::currentDateTime());
        ui->groupBox->setTitle("Check In/Out Information");

        this->setWindowTitle("Edit Check In/Out Information");
    }

}
