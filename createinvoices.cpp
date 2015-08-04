#include "createinvoices.h"
#include "ui_createinvoices.h"

#include "dbmysql.h"
#include <QDebug>
#include <QScrollBar>
#include <QMessageBox>
#include <QHash>
#include <QCompleter>
#include <QStandardItem>

#include "add_new_customer.h"
#include "apputils.h"

CreateInvoices::CreateInvoices(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CreateInvoices)
{
    ui->setupUi(this);
    ui->invoice_no->setText(this->genInvoiceNumber());
    ui->invoic_date->setDateTime(QDateTime::currentDateTime());
    ui->serviceTax->setText("0");
    ui->serviceTaxFood->setText("0");
}

CreateInvoices::~CreateInvoices()
{
    delete ui;
}


QString CreateInvoices::getInvoiceNumberFromBookingId(int booking_id)
{
    DbMysql* d = DbMysql::getInstance();
    QMessageBox msgBox;
    QString inv_num = "";
    if(!d->getConnection().open())
    {
        msgBox.critical(this,"Error","Failed to connect database.1");
        return "Unknown :( ";
    }
    else
    {

        QSqlQuery query1( "select inv_number from invoice where status=1 and booking_id="+QString::number(booking_id)+" limit 1;" ,d->getConnection());

        if(!query1.isActive())
        {
            msgBox.critical(this,"Error","Failed to connect database.");
            return "unknown :( ";
        }
        else
        {
            if(query1.size() > 0){
                query1.next();
                return query1.value(0).toString();
            }
            else return "unknown :( ";
        }
    }
}

bool CreateInvoices::isInvoicesCreated(int booking_id)
{
    DbMysql* d = DbMysql::getInstance();
    QMessageBox msgBox;
    QString inv_num = "";
    if(!d->getConnection().open())
    {
        msgBox.critical(this,"Error","Failed to connect database.1");
        return true;
    }
    else
    {

        QSqlQuery query1( "select * from invoice where status=0 and booking_id="+QString::number(booking_id)+";" ,d->getConnection());
        qDebug()<<query1.lastQuery();
        qDebug()<<"Size:"<<query1.size();

        if(!query1.isActive())
        {
            msgBox.critical(this,"Error","Failed to connect database.");
            return true;
        }
        else
        {
            if(query1.size() > 0)
            {
                return true;
            }
            else return false;
        }
    }
}

QString CreateInvoices::genInvoiceNumber()
{
    DbMysql* d = DbMysql::getInstance();
    QMessageBox msgBox;
    QString inv_num = "";
    if(!d->getConnection().open())
    {
        msgBox.critical(this,"Error","Failed to connect database.1");
    }
    else
    {

        QSqlQuery query1( "select max(inv_id) from invoice;" ,d->getConnection());

        if(!query1.isActive())
        {
            msgBox.critical(this,"Error","Failed to connect database.");
        }
        else
        {
            while(query1.next())
            {
                qDebug()<<"Val:"<<query1.value(0).toInt();
                inv_num = "HPI/"+QString::number(QDate::currentDate().year())+"/"+"#"+QString("%1").arg(query1.value(0).toInt()+1, 5, 10, QChar('0'));
            }

        }
    }
    return inv_num;
}

bool CreateInvoices::checkIfBookingAdded(int log_id) {
    int row_count = ui->lodging_bill->rowCount();
    qDebug()<<"Row:"<<row_count<<"Log:"<<log_id;
    for(int i=0;i<row_count;i++){
        qDebug()<<"check:"<<ui->lodging_bill->item(i,0)->data(Qt::UserRole).toInt();
        if(ui->lodging_bill->item(i,0)->data(Qt::UserRole).toInt() == log_id){
            return true;
        }
    }
    return false;
}

/*
void CreateInvoices::on_add_booking_clicked()
{
    DbMysql* d = DbMysql::getInstance();
    QMessageBox msgBox;
    if(checkIfBookingAdded(ui->log_id->text().toInt())){
        return;
    }

    if(!d->getConnection().open())
    {
        msgBox.critical(this,"Error","Failed to connect database.1");
    }
    else
    {

        QSqlQuery query1( "SELECT booking_log.log_id,details.customer_name,details.email,details.phone,details.address,details.room_number,details.rate_per_room,details.room_type,details.booking_from,details.booking_to,booking_log.check_in_time,booking_log.check_out_time,booking_log.reg_serial,details.id_type,details.id_serial,details.nop,details.ps_type,details.booking_id FROM booking_log left join ( select booking.booking_id,customers.customer_name,customers.email,customers.address,customers.phone,rooms.room_number,rooms.rate_per_room,rooms.room_type,customers.id_type,customers.id_serial,booking.booking_from,booking.booking_to,booking.nop,payment_status.ps_type from booking left join rooms on booking.room_id = rooms.room_id left join customers on customers.customer_id = booking.customer_id left join payment_status on payment_status.ps_id = booking.payment_status) as details on booking_log.booking_id = details.booking_id where log_id="+ui->log_id->text()+";" ,d->getConnection());

        if(!query1.isActive())
        {
            msgBox.critical(this,"Error","Failed to connect database."); //"yyyy-MM-dd hh:mm:ss"
        }
        else
        {
            while(query1.next())
            {
                if(ui->customer_name->text().length() > 0 && ui->customer_name->text() != query1.value("customer_name").toString()) {
                    msgBox.critical(this,"Error","This booking booked by another customer. You can't create single invoice booked by different customers. Please create another invoice for that. ");
                    return;
                }
                qDebug()<<query1.value("check_in_time").toString();
                ui->check_in_time->setDateTime(query1.value("check_in_time").toDateTime());
                ui->check_out_time->setDateTime(QDateTime::fromString(query1.value("check_out_time").toString(),"yyyy-MM-dd hh:mm:ss"));
                ui->reg_serial->setText(query1.value("reg_serial").toString());
                ui->customer_name->setText(query1.value("customer_name").toString());
                ui->address->setText(query1.value("address").toString());

                ui->email->setText(query1.value("email").toString());
                ui->phone->setText(query1.value("phone").toString());
                ui->address->setText(query1.value("address").toString());
                ui->id_type->setText(query1.value("id_type").toString());
                ui->id_serial->setText(query1.value("id_serial").toString());

                int row_count = ui->lodging_bill->rowCount();
                int row = 0 ;
                row_count+=1;
                if(row_count == 0)
                {
                    row = 0;
                }
                else
                {
                    row=row_count-1;
                }
                ui->lodging_bill->setRowCount(row_count);

                QStringList columnVals;
                columnVals<<query1.value("room_number").toString();
                columnVals<<query1.value("room_type").toString();
                columnVals<<query1.value("booking_from").toString();
                columnVals<<query1.value("booking_to").toString();
                //columnVals<<query1.value("booking_from").toString();
                int numDays = QDate::fromString(query1.value("booking_from").toString(),"yyyy-MM-dd").daysTo(QDate::fromString(query1.value("booking_to").toString(),"yyyy-MM-dd"))+1;
                columnVals<<QString::number(numDays);
                columnVals<<QString::number(query1.value("rate_per_room").toDouble(),'f',2);
                columnVals<<QString::number(query1.value("rate_per_room").toDouble()*numDays,'f',2);
                int column = 0 ;

                qDebug()<<columnVals;

                foreach(QString str,columnVals)
                {
                    QTableWidgetItem *item;
                    if(column == 5) {
                        item = new QTableWidgetItem("editable");
                    }
                    else {
                        item = new QTableWidgetItem("non editable");
                        item->setFlags(item->flags() & ~Qt::ItemIsEditable);
                    }
                    item->setData(Qt::UserRole,QVariant::fromValue(query1.value("log_id").toInt()));
                    item->setText(str);
                    qDebug()<<str<<" "<<row<<" "<<column;



                    ui->lodging_bill->setItem(row,column,item);
                    column++;
                }

                ui->lodging_bill->resizeColumnsToContents();
            }

        }
    }

    int totalFoodAmount = 0;
    qDebug()<<"Total:"<<ui->foodTotalAmount->text();
    QSqlQuery query2( "SELECT foods.food_name,food_orders.qty,food_orders.order_time,food_orders.amount FROM food_orders left join foods on foods.food_id=food_orders.food_id where booking_id="+ui->log_id->text()+" order by order_time desc;",d->getConnection());
    qDebug()<<query2.lastQuery();
    if(!query2.isActive())
    {
        msgBox.critical(this,"Error","Failed to connect database.");
    }
    else
    {
        int row = 0 ;
        row = ui->foodList->rowCount();
        ui->foodList->setRowCount(row+query2.size());

        //ui->orderTable->clear();
        qDebug()<<query2.size();
        qDebug()<<query2.lastQuery();

        while(query2.next())
        {

            int columnSize = ui->foodList->columnCount();
            qDebug()<<"Column Size:"<<columnSize;

            for( int column = 0; column < columnSize; column++)
            {
                QTableWidgetItem *newItem = new QTableWidgetItem();
                newItem->setData(Qt::UserRole,QVariant::fromValue(query2.value(0).toInt()));

                //newItem->setData(Qt::,QVariant::fromValue(query1.value(columnSize).toInt()));
                newItem->setText(query2.value(column).toString());
                ui->foodList->setItem(row, column, newItem);

            }

            totalFoodAmount+= query2.value(3).toInt();

            row++;
        }
        //ui->foodList->resizeColumnsToContents();
        ui->foodList->horizontalHeader()->setHighlightSections(false);
        ui->foodList->horizontalHeader()->setStyleSheet("QHeaderView::section { background-color:#0B2161;color:#fff; }");

    }
    qDebug()<<"Total:"<<ui->foodTotalAmount->text().toDouble();
    ui->foodTotalAmount->setText(QString::number(totalFoodAmount+ui->foodTotalAmount->text().toDouble(),'f',2));
    double service_tax_food = (ui->foodTotalAmount->text().toDouble()*ui->serviceTaxFood->text().toDouble())/100;
    ui->taxAmountFood->setText(QString::number(service_tax_food,'f',2));
    double grand_total_food = service_tax_food+ui->foodTotalAmount->text().toDouble();
    ui->grandTotalFood->setText(QString::number(grand_total_food,'f',2));
}
*/

void CreateInvoices::addBookingData(int booking_id)
{
    DbMysql* d = DbMysql::getInstance();
    QMessageBox msgBox;

    qDebug()<<"CreateInvoices::addBookingData:   BookingId:"<<booking_id;
    this->m_booking_id = booking_id;

    if(!d->getConnection().open())
    {
        msgBox.critical(this,"Error","Failed to connect database.1");
    }
    else
    {

        QSqlQuery query1( "SELECT * from booking_details where booking_id="+QString::number(booking_id)+";" ,d->getConnection());

        if(!query1.isActive())
        {
            msgBox.critical(this,"Error","Failed to connect database."); //"yyyy-MM-dd hh:mm:ss"
            qDebug()<<query1.lastQuery();
            qDebug()<<query1.lastError();
        }
        else
        {
            query1.next();


            qDebug()<<query1.value("in_time").toString();
            ui->check_in_time->setDateTime(query1.value("in_time").toDateTime());
            ui->check_out_time->setDateTime(query1.value("out_time").toDateTime());
            ui->reg_serial->setText(query1.value("reg_serial").toString());
            ui->customer_name->setText(query1.value("customer_name").toString());
            //ui->address->setText(query1.value("address").toString());

            ui->email->setText(query1.value("email").toString());
            ui->phone->setText(query1.value("phone").toString());
            ui->address->setText(query1.value("address").toString());
            ui->id_type->setText(query1.value("id_type").toString());
            ui->id_serial->setText(query1.value("id_serial").toString());

            QSqlQuery query2( "SELECT room_number,room_type,booking_from,booking_to,DATEDIFF(booking_to,booking_from)+1 as days,room_rate,(room_rate*(DATEDIFF(booking_to,booking_from)+1)) as amount from room_booking_details where booking_id="+QString::number(booking_id)+";" ,d->getConnection());

            if(!query2.isActive())
            {
                msgBox.critical(this,"Error","Failed to connect database."); //"yyyy-MM-dd hh:mm:ss"
                qDebug()<<query2.lastQuery();
                qDebug()<<query2.lastError();
            }
            else
            {

                ui->lodging_bill->setRowCount(query2.size());
                qDebug()<<"Row SIze:"<<ui->lodging_bill->rowCount();
                int row = 0 ;
                int col = ui->lodging_bill->columnCount();
                while(query2.next())
                {
                    for(int i=0;i<col;i++)
                    {
                        qDebug()<<"CreateInvoices::addBookingData:   row:"<<row<<" col:"<<i;;
                        QTableWidgetItem* item = new QTableWidgetItem();
                        item->setText(query2.value(i).toString());
                        qDebug()<<" val:"<<item->text();

                        ui->lodging_bill->setItem(row,i,item);
                    }
                    row++;
                }
            }
        }

        ui->lodging_bill->resizeColumnsToContents();
        int totalFoodAmount = 0;

        QSqlQuery query2( "SELECT food_name,rate,qty,order_time,(rate*qty) as amount  FROM food_orders where booking_id="+QString::number(booking_id)+" order by order_time desc;",d->getConnection());
        qDebug()<<query2.lastQuery();
        if(!query2.isActive())
        {
            msgBox.critical(this,"Error","Failed to connect database.");
        }
        else
        {
            int row = 0 ;
            row = ui->foodList->rowCount();
            ui->foodList->setRowCount(row+query2.size());

            //ui->orderTable->clear();
            qDebug()<<query2.size();
            qDebug()<<query2.lastQuery();

            while(query2.next())
            {

                int columnSize = ui->foodList->columnCount();
                qDebug()<<"Column Size:"<<columnSize;

                for( int column = 0; column < columnSize; column++)
                {
                    QTableWidgetItem *newItem = new QTableWidgetItem();
                    newItem->setData(Qt::UserRole,QVariant::fromValue(query2.value(0).toInt()));

                    //newItem->setData(Qt::,QVariant::fromValue(query1.value(columnSize).toInt()));
                    newItem->setText(query2.value(column).toString());
                    ui->foodList->setItem(row, column, newItem);

                }

                totalFoodAmount+= query2.value(4).toInt();

                row++;
            }
            ui->foodList->resizeColumnsToContents();
            ui->foodList->horizontalHeader()->setHighlightSections(false);
            //ui->foodList->horizontalHeader()->setStyleSheet("QHeaderView::section { background-color:#0B2161;color:#fff; }");

        }

        ui->foodTotalAmount->setText(QString::number(totalFoodAmount+ui->foodTotalAmount->text().toInt(),'f',2));
        double service_tax_food = (ui->foodTotalAmount->text().toDouble()*ui->serviceTaxFood->text().toDouble())/100;
        ui->taxAmountFood->setText(QString::number(service_tax_food,'f',2));
        double grand_total_food = service_tax_food+ui->foodTotalAmount->text().toDouble();
        ui->grandTotalFood->setText(QString::number(grand_total_food,'f',2));


    }
}




void CreateInvoices::on_lodging_bill_itemChanged(QTableWidgetItem *item)
{
    if(item->column()==6)
    {
        double room_rate = item->text().toDouble();
        double days = ui->lodging_bill->item(item->row(),item->column()-1)->text().toDouble();

        QTableWidgetItem* item1 = new QTableWidgetItem();
        item1->setFlags(item->flags() & ~Qt::ItemIsEditable);
        item1->setText(QString::number(room_rate*days,'f',2));
        ui->lodging_bill->setItem(item->row(),item->column()+1,item1);


        double total = ui->totalAmount->text().toDouble();

        total+=item->text().toDouble();
        ui->totalAmount->setText(QString::number(total,'f',2));
    }
    //qDebug()<<item->text();
}

void CreateInvoices::on_totalAmount_textChanged(const QString &arg1)
{

    double tamt = arg1.toDouble();
    double serviceTax = ui->serviceTax->text().toDouble() *tamt/100;
    ui->taxAmount->setText(QString::number(serviceTax,'f',2));
    ui->grandTotal->setText(QString::number(serviceTax+tamt,'f',2));

}

void CreateInvoices::on_serviceTax_textChanged(const QString &arg1)
{
    double tamt = ui->totalAmount->text().toDouble();
    double serviceTax = arg1.toDouble() * tamt /100;
    ui->taxAmount->setText(QString::number(serviceTax,'f',2));
    ui->grandTotal->setText(QString::number(serviceTax+tamt,'f',2));
}

void CreateInvoices::on_serviceTaxFood_textChanged(const QString &arg1)
{
    double tamt = ui->foodTotalAmount->text().toDouble();
    double serviceTax = arg1.toDouble() * tamt /100;
    ui->taxAmountFood->setText(QString::number(serviceTax,'f',2));
    ui->grandTotalFood->setText(QString::number(serviceTax+tamt,'f',2));
}



int CreateInvoices::genInvoiceId()
{
    DbMysql* d = DbMysql::getInstance();
    QMessageBox msgBox;

    if(!d->getConnection().open())
    {
        return 0;
    }
    else
    {
        QSqlQuery query1( "SELECT max(inv_id) from invoice;" ,d->getConnection());

        if(!query1.isActive())
        {
            return 0;
        }
        else
        {
            query1.next();
            return query1.value(0).toInt()+1;
        }
    }

}

void CreateInvoices::on_saveBtn_clicked()
{
    DbMysql* d = DbMysql::getInstance();
    QMessageBox msgBox;

    qDebug()<<"Clicked";

    if(isInvoicesCreated(this->m_booking_id))
    {
        msgBox.warning(this,"Create Invoice","Invoice already created with invoice number "+getInvoiceNumberFromBookingId(this->m_booking_id)+". To create new one please cancel the previous invoice.");
        return;
    }


    if(!d->getConnection().open())
    {
        msgBox.critical(this,"Error","Failed to connect database.1");
    }
    else
    {
        int invoice_id = genInvoiceId();

        if(invoice_id > 0 )
        {
            QSqlQuery query1( "insert into invoice values ("+QString::number(invoice_id)+",'"+ui->invoice_no->text()+"',CURRENT_TIMESTAMP(),"+ui->grandTotal->text()+","+ui->grandTotalFood->text()+","+ui->serviceTax->text()+","+ui->serviceTaxFood->text()+",0,"+QString::number(this->m_booking_id)+");" ,d->getConnection());
            qDebug()<<query1.lastError();
            qDebug()<<query1.lastQuery();

            if(!query1.isActive())
            {
                msgBox.critical(this,"Error","Failed to connect database."); //"yyyy-MM-dd hh:mm:ss"
            }
            else
            {
                msgBox.information(this,"Success","Invoice Created.");
                this->close();
            }
        }
    }
}

void CreateInvoices::on_cancelInvBtn_clicked()
{
    this->close();
}
