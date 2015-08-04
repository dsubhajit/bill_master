#include "createbooking.h"
#include "ui_createbooking.h"

#include "dbmysql.h"
#include <QDebug>
#include <QScrollBar>
#include <QMessageBox>
#include <QHash>
#include <QCompleter>
#include <QStandardItem>
#include <QModelIndexList>

#include "add_new_customer.h"
#include "apputils.h"

CreateBooking::CreateBooking(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CreateBooking)
{

    ui->setupUi(this);

    m_customer_id = -1;

    mEditMode = false;
    ui->from_date->setDate(QDate::currentDate());
    ui->to_date->setDate(QDate::currentDate());
    ui->advanceText->setVisible(false);
    ui->advanceMoney->setVisible(false);

    ui->single_day->setChecked(true);
    ui->to_date->setDisabled(true);
    this->addRoomsInForm();
    this->addCustomersInForm();
    this->addStatusFields();


}

void CreateBooking::setEditMode(bool mode)
{
    this->mEditMode = true;

    this->setWindowTitle("Edit Booking");

    DbMysql* d = DbMysql::getInstance();
    QMessageBox msgBox;

    if(!d->getConnection().open())
    {
        msgBox.critical(this,"Error","Failed to connect database.1");
    }
    else
    {

        QSqlQuery query1( "select * from booking_details where booking_id="+QString::number(this->mBookingId)+";" ,d->getConnection());
        QSqlQuery query2( "select * from booking where booking_id="+QString::number(this->mBookingId)+";" ,d->getConnection());
        QSqlQuery query3( "SELECT room_bookings.*,rooms.room_number FROM `room_bookings`left join rooms on rooms.room_id = room_bookings.room_id where booking_id="+QString::number(this->mBookingId)+";" ,d->getConnection());
        if((!query1.isActive() || !query2.isActive()) || !query3.isActive())
        {
            msgBox.critical(this,"Error","Failed to connect database.");
        }
        else
        {
            query1.next();
            query2.next();
            if(query1.value("booking_from").toDate() == query1.value("booking_to").toDate())
            {
                ui->single_day->setChecked(true);
            }
            else
            {
                ui->multiple_day->setChecked(true);
                ui->to_date->setDisabled(false);
            }

            mBookingFrom = query1.value("booking_from").toDate();
            mBookingTo = query1.value("booking_to").toDate();

            ui->from_date->setDate(query1.value("booking_from").toDate());
            ui->to_date->setDate(query1.value("booking_to").toDate());

            ui->c_customer_name->setText(query1.value("customer_name").toString());
            //ui->c_customer_name->completer()->setCurrentRow();
            QStandardItemModel* md = (QStandardItemModel*)ui->c_customer_name->completer()->model();
            qDebug() <<md->rowCount();
            QModelIndexList ml = md->match(md->index(0,0),Qt::UserRole,QVariant::fromValue(query2.value("customer_id").toInt()));

            if(ml.size() > 0)
            {
                ui->c_customer_name->completer()->activated(ml.at(0));
            }

            ui->customer_status->setCurrentIndex(query2.value("customer_status").toInt()-1);
            ui->check_in_date_time->setDateTime(query2.value("in_time").toDateTime());
            ui->num_of_person->setText(query2.value("nop").toString());
            ui->persons_name->setPlainText(query2.value("persons_name").toString());
            ui->comments->setPlainText(query2.value("commnets").toString());

            int i = 0;

            ui->room_status_table->setRowCount(query3.size());

            while(query3.next())
            {
                QTableWidgetItem* tItem  = new QTableWidgetItem();
                tItem->setText(query3.value("room_number").toString());
                tItem->setFlags(tItem->flags() & ~Qt::ItemIsEditable);

                QTableWidgetItem* tItem1  = new QTableWidgetItem();
                tItem1->setText(QString::number(query3.value("room_rate").toDouble(),'f',2));


                QTableWidgetItem* tItem2  = new QTableWidgetItem();
                tItem2->setText(query3.value("booking_from").toString());
                tItem2->setFlags(tItem2->flags() & ~Qt::ItemIsEditable);

                QTableWidgetItem* tItem3  = new QTableWidgetItem();
                if(ui->single_day->isChecked())
                {
                    tItem3->setText("");
                }
                if(ui->multiple_day->isChecked())
                {
                    tItem3->setText(query3.value("booking_to").toString());
                }

                tItem3->setFlags(tItem3->flags() & ~Qt::ItemIsEditable);


                QTableWidgetItem* tItem4  = new QTableWidgetItem();
                tItem4->setText("Available");
                tItem4->setFlags(tItem4->flags() & ~Qt::ItemIsEditable);

                ui->room_status_table->setItem(i,0,tItem);
                ui->room_status_table->setItem(i,1,tItem1);
                ui->room_status_table->setItem(i,2,tItem2);
                ui->room_status_table->setItem(i,3,tItem3);
                ui->room_status_table->setItem(i,4,tItem4);
                i++;

                for(int j= 0 ;j<ui->room_list->count();j++)
                {
                    if(ui->room_list->item(j)->data(Qt::UserRole).toInt() == query3.value("room_id").toInt())
                    {
                        ui->room_list->item(j)->setSelected(true);
                    }
                }


            }
            ui->advPayGBox->setVisible(false);
            ui->room_status_table->resizeColumnsToContents();


        }
    }
}

void CreateBooking::setBookingId(int id)
{

    this->mBookingId = id;
}

CreateBooking::~CreateBooking()
{
    delete ui;
}

void CreateBooking::addStatusFields ()
{
    DbMysql* d = DbMysql::getInstance();
    QMessageBox msgBox;

    if(!d->getConnection().open())
    {
        msgBox.critical(this,"Error","Failed to connect database.1");
    }
    else
    {

        QSqlQuery query1( "select * from customer_status;" ,d->getConnection());
        if(!query1.isActive())
        {
            msgBox.critical(this,"Error","Failed to connect database.");
        }
        else
        {
            //ui->roomsList->addItem("---- Select ----",QVariant::fromValue(-1));
            while(query1.next())
            {
                ui->customer_status->addItem(query1.value(1).toString(),QVariant::fromValue(query1.value(0).toInt()));
            }

        }
        QSqlQuery query2( "select * from payment_status;" ,d->getConnection());
        if(!query2.isActive())
        {
            msgBox.critical(this,"Error","Failed to connect database.");
        }
        else
        {
            //ui->roomsList->addItem("---- Select ----",QVariant::fromValue(-1));
            while(query2.next())
            {
                ui->payment_status->addItem(query2.value(1).toString(),QVariant::fromValue(query2.value(0).toInt()));
            }

        }
        QSqlQuery query3( "select * from booking_status;" ,d->getConnection());
        if(!query3.isActive())
        {
            msgBox.critical(this,"Error","Failed to connect database.");
        }
        else
        {
            //ui->roomsList->addItem("---- Select ----",QVariant::fromValue(-1));
            while(query3.next())
            {
                ui->booking_status->addItem(query3.value(1).toString(),QVariant::fromValue(query3.value(0).toInt()));
            }

        }

    }


}

void CreateBooking::addCustomersInForm()
{

    DbMysql* d = DbMysql::getInstance();
    QMessageBox msgBox;

    QCompleter* cl = new QCompleter(this);
    QStandardItemModel *model = new QStandardItemModel(cl);


    //ui->roomsList->setCurrentText("-----Select-----");
    if(!d->getConnection().open())
    {
        msgBox.critical(this,"Error","Failed to connect database.1");
    }
    else
    {

        QSqlQuery query1( "select customer_id,customer_name from customers order by customer_id desc;" ,d->getConnection());
        if(!query1.isActive())
        {
            msgBox.critical(this,"Error","Failed to connect database.");
        }
        else
        {
            //ui->roomsList->addItem("---- Select ----",QVariant::fromValue(-1));
            while(query1.next())
            {
                QStandardItem *item = new QStandardItem;
                item->setText(query1.value(1).toString());
                item->setData(query1.value(0).toInt(), Qt::UserRole);
                model->appendRow(item);

            }

        }

    }
    cl->setCaseSensitivity(Qt::CaseInsensitive);
    cl->setModel(model);
    ui->c_customer_name->setCompleter(cl);

    //ui->c_customer_name
    connect(cl, SIGNAL(activated(QModelIndex)),this, SLOT(onItemActivated(QModelIndex)));
}

void CreateBooking::addRoomsInForm()
{
    DbMysql* d = DbMysql::getInstance();
    QMessageBox msgBox;

    //ui->roomsList->setCurrentText("-----Select-----");
    if(!d->getConnection().open())
    {
        msgBox.critical(this,"Error","Failed to connect database.1");
    }
    else
    {

        QSqlQuery query1( "select room_id,room_number,room_type from rooms order by room_id;" ,d->getConnection());
        if(!query1.isActive())
        {
            msgBox.critical(this,"Error","Failed to connect database.");
        }
        else
        {
            //ui->roomsList->addItem("---- Select ----",QVariant::fromValue(-1));
            int i =0 ;
            while(query1.next())
            {
                QListWidgetItem *item = new QListWidgetItem();
                item->setText(query1.value(1).toString()+"  ( "+query1.value(2).toString()+" )");
                item->setData(Qt::UserRole,QVariant::fromValue(query1.value(0).toInt()));
                ui->room_list->insertItem(i,item);
                i++;
                //ui->roomsList->addItem(query1.value(1).toString(),QVariant::fromValue(query1.value(0).toInt()));
            }

        }

    }
}

/*
void CreateBooking::on_roomsList_currentIndexChanged(int index)
{

    QMessageBox msgBox;
    ui->status->setText("Please wait ....");
    qDebug()<<"CurIndex:"<<ui->roomsList->itemData(index).toInt();
    int cur_room_id = ui->roomsList->itemData(index).toInt();
    if(cur_room_id>0)
    {
        DbMysql* d = DbMysql::getInstance();
        QMessageBox msgBox;

        //ui->roomsList->setCurrentText("-----Select-----");
        if(!d->getConnection().open())
        {
            msgBox.critical(this,"Error","Failed to connect database.1");
        }
        else
        {

            QSqlQuery query1( "select * from rooms where room_id="+QString::number(cur_room_id)+";" ,d->getConnection());
            qDebug()<<query1.lastQuery();
            if(!query1.isActive())
            {
                msgBox.critical(this,"Error","Failed to connect database.");
            }
            else
            {
                query1.first();
                ui->single_beds->setText(query1.value("single_beds").toString());
                ui->double_beds->setText(query1.value("double_beds").toString());
                ui->extra_beds->setText(query1.value("extra_beds").toString());
                ui->equipment_text->setText(query1.value("room_equip").toString());
                ui->description_test->setText(query1.value("room_desc").toString());
            }
        }
        //msgBox.critical(this,"Error","Please select valid room.");
    }
    else
    {

    }
    ui->status->setText("");

}
*/

void CreateBooking::on_from_date_dateChanged(const QDate &date)
{
    int val = date.daysTo(ui->to_date->date());    
    val+=1;
    if(val==0) ui->days_count->setText(QString::number(val));
    ui->to_date->setDate(ui->from_date->date().addDays(1));
}

void CreateBooking::on_to_date_dateChanged(const QDate &date)
{

    int val = ui->from_date->date().daysTo(date);
    val+=1;
    if(ui->single_day->isChecked()){
        ui->days_count->setText(QString::number(1));
    }
    else
        if(val>=0) ui->days_count->setText(QString::number(val));
}

void CreateBooking::onItemActivated(const QModelIndex &index)
{
    int customer_id = index.data(Qt::UserRole).toInt();
    DbMysql* d = DbMysql::getInstance();
    QMessageBox msgBox;

    m_customer_id = customer_id;

    qDebug()<< m_customer_id;

    qDebug()<<ui->c_customer_name->completer()->currentIndex().data(Qt::UserRole).toInt();

    if(!d->getConnection().open())
    {
        msgBox.critical(this,"Error","Failed to connect database.1");
    }
    else
    {

        QSqlQuery query1( "select * from customers where customer_id="+QString::number(customer_id)+";" ,d->getConnection());
        if(!query1.isActive())
        {
            msgBox.critical(this,"Error","Failed to connect database.");
        }
        else
        {
            qDebug()<<query1.lastQuery();
            while(query1.next())
            {
                ui->c_customer_add->setText(query1.value("address").toString());
                ui->c_customer_email->setText(query1.value("email").toString());
                ui->c_customer_phone->setText(query1.value("phone").toString());
                ui->c_customer_id_type->setText(query1.value("id_type").toString());
                ui->c_customer_id_serial->setText(query1.value("id_serial").toString());
            }
        }
    }
}



void CreateBooking::on_addNewCustomer_clicked()
{
   add_new_customer* cs = new add_new_customer();
   cs->setAttribute(Qt::WA_DeleteOnClose);
   cs->setBookingCreateParent(this);
   cs->show();

}

void CreateBooking::on_cancelBtn_clicked()
{
    this->close();
}

bool CreateBooking::checkBookingAvail(QString date_from, QString date_to, int room_id)
{
    DbMysql* d = DbMysql::getInstance();
    QMessageBox msgBox;

    if(!d->getConnection().open())
    {
        //msgBox.critical(this,"Error","Failed to connect database.1");
        return false;
    }
    else
    {
        QSqlQuery query1("SELECT * FROM `room_bookings` where room_id="+QString::number(room_id)+" and (( booking_from>='"+date_from+"' and booking_to <= '"+date_to+"') or (booking_from>='"+date_from+"' and booking_from <= '"+date_to+"') or (booking_to>='"+date_from+"' and booking_to <= '"+date_to+"'));",d->getConnection());
        qDebug()<<query1.lastQuery();

        if(query1.isActive())
        {
            if(query1.size() > 0)
            {
                return false;
            }
            else return true;
        }
        else return false;
    }

}

int CreateBooking::genBookingId()
{
    DbMysql* d = DbMysql::getInstance();

    if(!d->getConnection().open())
    {
        //msgBox.critical(this,"Error","Failed to connect database.1");
        return 0;
    }
    else
    {
        QSqlQuery query1("select max(booking_id) from booking;",d->getConnection());

        if(query1.isActive())
        {
            query1.next();
            return query1.value(0).toInt()+1;
        }
        else return 0;
    }
}

void CreateBooking::on_saveBookingBtn_clicked()
{

    QMessageBox msgBox;
    //int room_id = ui->roomsList->currentData().toInt();
    //ui->customer_name->completer()->currentIndex()
    //int customer_id = ui->c_customer_name->completer()->currentIndex().data(Qt::UserRole).toInt();
    int customer_id = m_customer_id;
    //customer_id = ui->c_customer_name->completer()->currentIndex().data(Qt::UserRole).toInt();
    /*
    QModelIndex index = ui->c_customer_name->completer()->currentIndex();
    if (index.isValid()) {
        int row = index.row();
        customer_id =  ui->c_customer_name->completer->completionModel()->index(row, 0).data().toInt();
        //qDebug() << key;
    }
    */
    if(customer_id < 0)
    {
        msgBox.warning(this,"Save Booking","Wrong customer details.");
        return;
    }

    qDebug()<<"Customer Id:"<<customer_id;
    QDate booking_date_from = ui->from_date->date();
    QDate booking_date_to = ui->to_date->date();

    if(ui->room_status_table->rowCount() <= 0 )
    {
        msgBox.warning(this,"Save Booking","Please check the availability of rooms before saving the booking.");
        return;
    }

    if(!this->checkRoomAvailability())
    {
        //msgBox.warning(this,"Save Booking","Please check the availability of rooms before saving the booking.");
        return;
    }




    int customer_status = ui->customer_status->currentData().toInt();
    int num_of_persons = ui->num_of_person->text().toInt();
    int booking_status = ui->booking_status->currentData().toInt();
    QString name_of_persons = ui->persons_name->toPlainText();
    QString comments = ui->comments->toPlainText();
    int payment_status  = ui->payment_status->currentData().toInt();

    QString check_in_time;
    if(customer_status == 2)
    {
        check_in_time = ui->check_in_date_time->dateTime().toString("yyyy-MM-dd hh:mm:ss");
    }

    DbMysql* d = DbMysql::getInstance();

    if(!d->getConnection().open())
    {
        msgBox.critical(this,"Error","Failed to connect database.1");
    }
    else
    {
        int booking_id = 0;
        if(mEditMode)
        {
            booking_id=mBookingId;
        }
        else
            booking_id = this->genBookingId();


        //QSqlQuery query2("INSERT INTO `room_booking` VALUES (DEFAULT, '"+QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss")+"', '"+QString::number(customer_id)+"', '"+booking_date_from.toString("yyyy-MM-dd")+"', '"+booking_date_to.toString("yyyy-MM-dd")+"', '"+QString::number(room_id)+"', '"+QString::number(booking_status)+"', '"+QString::number(customer_status)+"', '"+QString::number(payment_status)+"', '"+QString::number(num_of_persons)+"','"+name_of_persons+"','"+comments+"');",d->getConnection());


        QString sql0;

        if(mEditMode)
        {
            sql0 = "UPDATE `booking` set customer_id="+QString::number(customer_id)+",booking_from='"+booking_date_from.toString("yyyy-MM-dd")+"',booking_to='"+booking_date_to.toString("yyyy-MM-dd")+"',booking_status="+QString::number(booking_status)+", customer_status='"+QString::number(customer_status)+"',nop='"+QString::number(num_of_persons)+"',persons_name='"+name_of_persons+"',commnets='"+comments+"' where booking_id="+QString::number(mBookingId)+";";
        }
        else
        {
            sql0 = "INSERT INTO `booking` VALUES (DEFAULT, '"+QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss")+"', '"+QString::number(customer_id)+"', '"+booking_date_from.toString("yyyy-MM-dd")+"', '"+booking_date_to.toString("yyyy-MM-dd")+"','"+((check_in_time.length() > 0)?check_in_time:"NULL")+"','NULL','',"+QString::number(booking_status)+", '"+QString::number(customer_status)+"', '"+QString::number(payment_status)+"', '"+QString::number(num_of_persons)+"','"+name_of_persons+"','"+comments+"');";
        }

        QSqlQuery query1(sql0,d->getConnection());
        if(query1.isActive())
        {
            if(mEditMode)
            {
                QSqlQuery query11("delete from room_bookings where booking_id="+QString::number(mBookingId)+";",d->getConnection());
                if(!query11.isActive())
                {
                    qDebug()<<query11.lastQuery();
                }
            }


            QString sql = "Insert into room_bookings values ";


            QString sql2 = "Update rooms set room_state=1 where room_id in (";

            bool comaFlag = false;

            QList<QListWidgetItem*> room_items =  ui->room_list->selectedItems();
            ui->room_status_table->setRowCount(room_items.size());
            int row = 0;
            foreach (QListWidgetItem* item, room_items)
            {
                if(comaFlag)sql+=",";
                sql += "(DEFAULT,"+ui->room_status_table->item(row,1)->text()+","+item->data(Qt::UserRole).toString()+",'"+booking_date_from.toString("yyyy-MM-dd")+"', '"+((ui->single_day->isChecked())?booking_date_from.toString("yyyy-MM-dd"):booking_date_to.toString("yyyy-MM-dd"))+"',"+QString::number(booking_id)+")";
                sql2 += item->data(Qt::UserRole).toString();
                comaFlag = true;
                row++;
            }

            sql2+= ");";

            if(!mEditMode)
            {
                if(check_in_time.length() > 0)
                {
                    QSqlQuery query3(sql2,d->getConnection());
                }
            }

            if(ui->payment_status->currentData(Qt::UserRole).toInt() == 3)
            {

                QSqlQuery query4("insert into advance_payments values (DEFAULT,"+ui->advanceMoney->text()+","+QString::number(booking_id)+",NOW());",d->getConnection());
                if(!query4.isActive())
                {
                    qDebug()<<query4.lastQuery();
                    qDebug()<<query4.lastError();
                    return;
                }
            }


            QSqlQuery query2(sql,d->getConnection());
            if(query2.isActive())
            {
                if(mEditMode)
                {
                    msgBox.information(this,"Edit Booking","Update Booking done.");
                }
                else
                {
                    msgBox.information(this,"Booking","Booking done.");
                }

                this->mpParent->populateBookingLog();
                this->mpParent->createCalendar();

                this->close();
            }
            else
            {
                qDebug()<<query2.lastQuery();
                qDebug()<<query2.lastError();
                return;
            }



        }
        else
        {
            qDebug()<<query1.lastQuery();
            qDebug()<<query1.lastError();
            return;
        }

    }
}


bool CreateBooking::checkRoomAvailability()
{
    QMessageBox msgBox;

    QString booking_date_from = ui->from_date->date().toString("yyyy-MM-dd");
    QString booking_date_to = ui->to_date->date().toString("yyyy-MM-dd");

    QList<QListWidgetItem*> room_items =  ui->room_list->selectedItems();

    QString result= "";
    if(mEditMode)
    {
        foreach (QListWidgetItem* item, room_items)
        {
            QDate nf = ui->from_date->date();
            QDate nt = ui->to_date->date();
            QDate tmpFrom1,tmpTo1,tmpFrom2,tmpTo2;

            bool bookingAvl = false;
            if((nf <= mBookingFrom) && (nt >= mBookingTo))
            {
                tmpFrom1 = nf;
                tmpTo1 = mBookingFrom.addDays(-1);
                tmpFrom2 = mBookingTo.addDays(1);
                tmpTo2 = nt;
                if(checkBookingAvail(tmpFrom1.toString("yyyy-MM-dd"),tmpTo1.toString("yyyy-MM-dd"),item->data(Qt::UserRole).toInt()) && checkBookingAvail(tmpFrom2.toString("yyyy-MM-dd"),tmpTo2.toString("yyyy-MM-dd"),item->data(Qt::UserRole).toInt()))
                {
                    bookingAvl = true;
                }
                else bookingAvl = false;

            }
            else if( (nf <= mBookingFrom) && (nt <= mBookingTo) )
            {
                tmpFrom1 = nf;
                tmpTo1 = mBookingFrom.addDays(-1);
                bookingAvl = checkBookingAvail(tmpFrom1.toString("yyyy-MM-dd"),tmpTo1.toString("yyyy-MM-dd"),item->data(Qt::UserRole).toInt());
            }
            else if( (nf >= mBookingFrom) && (nt >= mBookingTo))
            {
                tmpFrom1 = mBookingTo.addDays(1);
                tmpTo1 = nt;
                bookingAvl = checkBookingAvail(tmpFrom1.toString("yyyy-MM-dd"),tmpTo1.toString("yyyy-MM-dd"),item->data(Qt::UserRole).toInt());
            }
            else {
                tmpFrom1 = nf;
                tmpTo1 = nt;
                bookingAvl = checkBookingAvail(tmpFrom1.toString("yyyy-MM-dd"),tmpTo1.toString("yyyy-MM-dd"),item->data(Qt::UserRole).toInt());
            }

            if(bookingAvl)
            {

            }
            else
            {

                result += "Room no. "+item->text()+" not available for booking.";
                msgBox.critical(this,"Save Booking",result);
                return false;
            }
        }
    }
    else
    {
        foreach (QListWidgetItem* item, room_items)
        {
            if(!this->checkBookingAvail(booking_date_from,booking_date_to,item->data(Qt::UserRole).toInt()))
            {
                result += "Room no. "+item->text()+" not available for booking.";
                msgBox.critical(this,"Save Booking",result);
                return false;
            }
            else
            {
                result += "Room no. "+item->text()+" available for booking.";
            }
        }
    }
    return true;
}

void CreateBooking::on_checkAvail_clicked()
{

    QMessageBox msgBox;

    QString booking_date_from = ui->from_date->date().toString("yyyy-MM-dd");
    QString booking_date_to = ui->to_date->date().toString("yyyy-MM-dd");

    QList<QListWidgetItem*> room_items =  ui->room_list->selectedItems();

    QString result= "";

    ui->room_status_table->setRowCount(room_items.size());

    int i =0;
    DbMysql* d = DbMysql::getInstance();
    foreach (QListWidgetItem* item, room_items)
    {

        double room_rate = 0;
        if(!d->getConnection().open())
        {
            msgBox.critical(this,"Error","Failed to connect database.1");
        }
        else
        {
            QSqlQuery query1("select rate_per_room from rooms where room_id="+item->data(Qt::UserRole).toString(),d->getConnection());
            if(query1.isActive())
            {
                query1.next();
                room_rate = query1.value(0).toDouble();
            }
        }


        QTableWidgetItem* tItem  = new QTableWidgetItem();
        tItem->setText(item->text());
        tItem->setFlags(item->flags() & ~Qt::ItemIsEditable);

        QTableWidgetItem* tItem1  = new QTableWidgetItem();
        tItem1->setText(QString::number(room_rate,'f',2));


        QTableWidgetItem* tItem2  = new QTableWidgetItem();
        tItem2->setText(booking_date_from);
        tItem2->setFlags(item->flags() & ~Qt::ItemIsEditable);

        QTableWidgetItem* tItem3  = new QTableWidgetItem();
        if(ui->single_day->isChecked())
        {
            tItem3->setText("");
        }
        if(ui->multiple_day->isChecked())
        {
            tItem3->setText(booking_date_to);
        }

        tItem3->setFlags(item->flags() & ~Qt::ItemIsEditable);

        QString status = "";
        if(mEditMode)
        {

            QDate nf = ui->from_date->date();
            QDate nt = ui->to_date->date();
            QDate tmpFrom1,tmpTo1,tmpFrom2,tmpTo2;

            bool bookingAvl = false;
            if((nf <= mBookingFrom) && (nt >= mBookingTo))
            {
                tmpFrom1 = nf;
                tmpTo1 = mBookingFrom.addDays(-1);
                tmpFrom2 = mBookingTo.addDays(1);
                tmpTo2 = nt;
                if(checkBookingAvail(tmpFrom1.toString("yyyy-MM-dd"),tmpTo1.toString("yyyy-MM-dd"),item->data(Qt::UserRole).toInt()) && checkBookingAvail(tmpFrom2.toString("yyyy-MM-dd"),tmpTo2.toString("yyyy-MM-dd"),item->data(Qt::UserRole).toInt()))
                {
                    bookingAvl = true;
                }
                else bookingAvl = false;

            }
            else if( (nf <= mBookingFrom) && (nt <= mBookingTo) )
            {
                tmpFrom1 = nf;
                tmpTo1 = mBookingFrom.addDays(-1);
                bookingAvl = checkBookingAvail(tmpFrom1.toString("yyyy-MM-dd"),tmpTo1.toString("yyyy-MM-dd"),item->data(Qt::UserRole).toInt());
            }
            else if( (nf >= mBookingFrom) && (nt >= mBookingTo))
            {
                tmpFrom1 = mBookingTo.addDays(1);
                tmpTo1 = nt;
                bookingAvl = checkBookingAvail(tmpFrom1.toString("yyyy-MM-dd"),tmpTo1.toString("yyyy-MM-dd"),item->data(Qt::UserRole).toInt());
            }
            else {
                tmpFrom1 = nf;
                tmpTo1 = nt;
                bookingAvl = checkBookingAvail(tmpFrom1.toString("yyyy-MM-dd"),tmpTo1.toString("yyyy-MM-dd"),item->data(Qt::UserRole).toInt());
            }

            if(bookingAvl)
                status = "Available";
            else
            {
                status = "Not Available";
                result += "Room no. "+item->text()+" not available for booking.";
            }

        }
        else
        {

            if(!this->checkBookingAvail(booking_date_from,booking_date_to,item->data(Qt::UserRole).toInt()))
            {
                result += "Room no. "+item->text()+" not available for booking.";
                status = "Not Available";
            }
            else
            {
                result += "Room no. "+item->text()+" available for booking.";
                status = "Available";
            }
        }

        QTableWidgetItem* tItem4  = new QTableWidgetItem();
        tItem4->setText(status);
        tItem4->setFlags(item->flags() & ~Qt::ItemIsEditable);

        ui->room_status_table->setItem(i,0,tItem);
        ui->room_status_table->setItem(i,1,tItem1);
        ui->room_status_table->setItem(i,2,tItem2);
        ui->room_status_table->setItem(i,3,tItem3);
        ui->room_status_table->setItem(i,4,tItem4);

        i++;
    }

    ui->room_status_table->resizeColumnsToContents();

}

void CreateBooking::on_customer_status_currentIndexChanged(int index)
{
    if(index == 1)
    {
        ui->check_in_label->show();
        ui->check_in_date_time->setDateTime(QDateTime::currentDateTime());
        ui->check_in_date_time->show();
        ui->check_in_date_time->setReadOnly(false);
    }
    else {
        ui->check_in_label->hide();
        ui->check_in_date_time->hide();
        ui->check_in_date_time->setReadOnly(true);
    }
}



void CreateBooking::on_multiple_day_clicked(bool checked)
{
    if(checked)
    {
        ui->to_date->setDisabled(false);
        ui->to_date->setDate(ui->from_date->date().addDays(1));
    }
    else ui->to_date->setDisabled(true);
}

void CreateBooking::on_single_day_clicked(bool checked)
{
    if(checked)
    {
        ui->to_date->setDisabled(true);
        ui->to_date->setDate(ui->from_date->date());
    }
    else ui->to_date->setDisabled(false);
}


void CreateBooking::setParent(BookingCalendar *aclndr)
{
    this->mpParent = aclndr;
}



void CreateBooking::on_payment_status_currentIndexChanged(int index)
{
    qDebug()<<"index:"<<index;
    if(ui->payment_status->itemData(index,Qt::UserRole).toInt() == 3)
    {
        ui->advanceText->setVisible(true);
        ui->advanceMoney->setVisible(true);
    }
    else
    {
        ui->advanceText->setVisible(false);
        ui->advanceMoney->setVisible(false);
    }
}
