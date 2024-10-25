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
    //ui->serviceTax->setText("0");

}

CreateInvoices::~CreateInvoices()
{
    delete ui;
}


QString CreateInvoices::getInvoiceNumberFromBookingId(int booking_id)
{
    DbMysql* d = DbMysql::getInstance();
    QMessageBox msgBox;    
    if(!d->getConnection().open())
    {
        msgBox.critical(this,"Error","Failed to connect database.1");
        return "Unknown :( ";
    }
    else
    {

        QSqlQuery query1( "select inv_number from invoice where status = 0 and booking_id="+QString::number(booking_id)+" limit 1;" ,d->getConnection());

        if(!query1.isActive())
        {
            qDebug()<<query1.lastError();
            msgBox.critical(this,"Error","Failed to connect database.");
            return "unknown :( ";
        }
        else
        {
            qDebug()<<query1.lastQuery();

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
        QDate curDate = QDate::currentDate();
        int startYear = curDate.year();
        int endYear = curDate.year()+1;

        if(curDate.month() >= 1 && curDate.month() < 4){
            endYear = startYear;
            startYear--;
        }

        QString fy_start = QString::number(startYear)+"-04-01";
        QString fy_end = QString::number(endYear)+"-03-31";

        QSqlQuery query1( "select max(inv_sl) from invoice where inv_date >='"+fy_start+"' and inv_date <= '"+fy_end+"';" ,d->getConnection());

        if(!query1.isActive())
        {
            msgBox.critical(this,"Error","Failed to connect database.");
        }
        else
        {
            while(query1.next())
            {
                qDebug()<<"Val:"<<query1.value(0).toInt();
                inv_num = "HPI/"+QString("%1").arg(query1.value(0).toInt()+1, 5, 10, QChar('0'))+"/"+QString::number(startYear)+"-"+QString::number(endYear);
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

            double total_tax_amount = 0;
            double total_amount = 0;

            qDebug()<<query1.value("in_time").toString();
            ui->booking_date->setDate(query1.value("booking_date").toDate());

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
            ui->gst_no->setText(query1.value("gstin_no").toString());

            QSqlQuery query2( "SELECT room_number,room_type,booking_from,booking_to,DATEDIFF(booking_to,booking_from)+1 as days,room_rate,(cgst+sgst)as gst,(room_rate*(DATEDIFF(booking_to,booking_from)+1)*(cgst/100)) as cgst,(room_rate*(DATEDIFF(booking_to,booking_from)+1)*(sgst/100)) as sgst,((room_rate*(DATEDIFF(booking_to,booking_from)+1)*(cgst+sgst)/100)+(room_rate*(DATEDIFF(booking_to,booking_from)+1))) as amount from room_booking_details where booking_id="+QString::number(booking_id)+";" ,d->getConnection());

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
                        if(i>=5)
                        {
                            item->setText(QString::number(query2.value(i).toDouble(),'f',2));
                        }
                        else item->setText(query2.value(i).toString());
                        qDebug()<<" val:"<<item->text();

                        ui->lodging_bill->setItem(row,i,item);
                    }
                    row++;
                    total_tax_amount += query2.value("cgst").toDouble() + query2.value("sgst").toDouble();
                    total_amount += query2.value("amount").toDouble() - (query2.value("cgst").toDouble() + query2.value("sgst").toDouble());

                }
            }


            ui->totalAmount->setText(QString::number(total_amount,'f',2));
            ui->taxAmount->setText(QString::number(total_tax_amount,'f',2));

            ui->grandTotal->setText(QString::number(total_amount+total_tax_amount,'f',2));
        }

        ui->lodging_bill->resizeColumnsToContents();
        double totalFoodAmount = 0;
        double totalTaxAmountFood  = 0;


        QSqlQuery query2( "SELECT hsn_code,food_name,order_time,rate,qty,cgst,sgst,tax_amount,total_amount  FROM food_order_detailsfull where booking_id="+QString::number(booking_id)+" order by order_time desc;",d->getConnection());
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
                    if(column >2){
                        newItem->setText(query2.value(column).toString()+".00");
                        newItem->setTextAlignment(Qt::AlignRight);
                    }
                    else newItem->setText(query2.value(column).toString());
                    ui->foodList->setItem(row, column, newItem);

                }

                totalFoodAmount+= query2.value(3).toDouble()*query2.value(4).toDouble();
                totalTaxAmountFood+= query2.value(7).toDouble();

                row++;
            }
            ui->foodList->resizeColumnsToContents();
            ui->foodList->horizontalHeader()->setHighlightSections(false);
            //ui->foodList->horizontalHeader()->setStyleSheet("QHeaderView::section { background-color:#0B2161;color:#fff; }");

        }

        ui->foodTotalAmount->setText(QString::number(totalFoodAmount,'f',2));
        ui->taxAmountFood->setText(QString::number(totalTaxAmountFood,'f',2));
        ui->grandTotalFood->setText(QString::number(totalFoodAmount+totalTaxAmountFood,'f',2));



    }
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

double CreateInvoices::getCgstTaxAmount()
{
    int rowSize = ui->foodList->rowCount();
    double cgst_tax = 0;

    for( int row = 0; row < rowSize; row++)
    {
        cgst_tax += (ui->foodList->item(row,3)->text().toDouble()*ui->foodList->item(row,4)->text().toDouble()*ui->foodList->item(row,5)->text().toDouble())/100;
    }


    rowSize = ui->lodging_bill->rowCount();


    for( int row = 0; row < rowSize; row++)
    {
        cgst_tax += ui->lodging_bill->item(row,7)->text().toDouble();
    }


    return cgst_tax;
}

double CreateInvoices::getSgstTaxAmount()
{
    int rowSize = ui->foodList->rowCount();
    double sgst_tax = 0;

    for( int row = 0; row < rowSize; row++)
    {
        sgst_tax += (ui->foodList->item(row,3)->text().toDouble()*ui->foodList->item(row,4)->text().toDouble()*ui->foodList->item(row,6)->text().toDouble())/100;
    }

    rowSize = ui->lodging_bill->rowCount();


    for( int row = 0; row < rowSize; row++)
    {
        sgst_tax += ui->lodging_bill->item(row,8)->text().toDouble();
    }



    return sgst_tax;
}

void CreateInvoices::on_saveBtn_clicked()
{
    DbMysql* d = DbMysql::getInstance();
    QMessageBox msgBox;

    QString inv_date = ui->invoic_date->dateTime().toString("yyyy-MM-dd hh:mm:ss");


    //qDebug()<<"Clicked "<<getCgstTaxAmount()<<" "<<getSgstTaxAmount();

    if(isInvoicesCreated(this->m_booking_id))
    {
        msgBox.warning(this,"Create Invoice","Invoice already created with invoice number "+getInvoiceNumberFromBookingId(this->m_booking_id)+". To create new one please delete the previous invoice.");
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

            //chnaged

            int inv_sl = ui->invoice_no->text().split("#", QString::SkipEmptyParts)[1].toInt();

            qDebug()<<"================================>";
            qDebug()<<inv_sl;

            QSqlQuery query1( "insert into invoice values ("+QString::number(invoice_id)+","+QString::number(inv_sl)+",'"+ui->invoice_no->text()+"','"+inv_date+"',"+ui->grandTotal->text()+","+ui->grandTotalFood->text()+",0,0,0,"+QString::number(this->m_booking_id)+");" ,d->getConnection());
            qDebug()<<query1.lastError();
            qDebug()<<query1.lastQuery();

            if(!query1.isActive())
            {
                msgBox.critical(this,"Error","Failed to connect database."); //"yyyy-MM-dd hh:mm:ss"
            }
            else
            {
                QSqlQuery query2("insert into gst_data  values(DEFAULT,"+QString::number(getCgstTaxAmount())+","+QString::number(getSgstTaxAmount())+","+QString::number(this->m_booking_id)+","+QString::number(invoice_id)+",'"+ui->invoice_no->text()+"',CURDATE());");
                if(!query2.isActive())
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
}

void CreateInvoices::on_cancelInvBtn_clicked()
{
    this->close();
}
