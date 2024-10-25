#include "invoices.h"
#include "ui_invoices.h"
#include "createinvoices.h"
#include "apputils.h"
#include "dbmysql.h"
#include "invoiceviewer.h"
#include <QtPrintSupport/QPrinter>
#include <QTextDocument>
#include <QtPrintSupport/QPrintDialog>
#include <QMessageBox>
#include <QPainter>
#include <QPrintDialog>
#include <QTextCursor>
#include <QMargins>
#include <QTextBlockFormat>
#include <QMap>


Invoices::Invoices(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Invoices)
{
    ui->setupUi(this);
    QStringList months;



    months<<"Jan"<<"Feb"<<"Mar"<<"Apr"<<"May"<<"Jun"<<"Jul"<<"Aug"<<"Sep"<<"Oct"<<"Nov"<<"Dec";
    int i=1;
    int cur_mnth = QDate::currentDate().month();

    foreach (QString mnth, months) {
        ui->monthList->addItem(mnth,QVariant::fromValue(i++));

    }
    ui->monthList->setCurrentIndex(cur_mnth-1);

    int cur_year= QDate::currentDate().year();

    for(int i=2000;i<2050;i++)
    {
        ui->yearList->addItem(QString::number(i),QVariant::fromValue(i));
    }
    ui->yearList->setCurrentIndex(cur_year-2000);

    //ui->lBill->setVisible(false);
    //ui->fBill->setVisible(false);
        //ui->textEdit->setHtml("<h1>Hello</h1>");
    createInvoiceTable();
}

Invoices::~Invoices()
{
    delete ui;
}

void Invoices::on_addInvoices_clicked()
{
    CreateInvoices *cl = new CreateInvoices();
    cl->show();
}

void Invoices::createInvoiceTable()
{

    int year = ui->yearList->currentData(Qt::UserRole).toInt();
    int month = ui->monthList->currentData(Qt::UserRole).toInt();

    DbMysql* d = DbMysql::getInstance();
    QMessageBox msgBox;

    if(!d->getConnection().open())
    {
        msgBox.critical(this,"Error","Failed to connect database.1");
    }
    else
    {

        QSqlQuery query1( "SELECT  inv_id,inv_number,booking_id,inv_date,customer_name,room_numbers,lodging_bill_total,fooding_bill_total,ps_type,status,booking_id from invoice_details inner join payment_status on payment_status.ps_id = invoice_details.payment_status where MONTH(inv_date)="+QString::number(month)+" and YEAR(inv_date)="+QString::number(year)+" order by inv_id desc",d->getConnection());
        qDebug()<<query1.lastQuery();
        if(!query1.isActive())
        {
            msgBox.critical(this,"Error","Failed to connect database.");
        }
        else
        {
            ui->invoiceTable->setRowCount(query1.size());
            int row = 0 ;
            while(query1.next())
            {
                int columnSize = ui->invoiceTable->columnCount();
                qDebug()<<"Column Size:"<<columnSize;
                for( int column = 0; column < columnSize; column++)
                {
                    QTableWidgetItem *newItem = new QTableWidgetItem();
                    newItem->setData(Qt::UserRole,QVariant::fromValue(query1.value(0).toInt()));

                    //newItem->setData(Qt::,QVariant::fromValue(query1.value(columnSize).toInt()));
                    newItem->setText(query1.value(column).toString());

                    if(query1.value("status").toInt() == 1)
                    {
                        QColor clr;
                        clr.setNamedColor("#FFB3AD");
                        newItem->setBackgroundColor(clr);
                    }

                    if(column==9)
                    {
                        newItem->setData(Qt::UserRole,QVariant::fromValue(query1.value(10).toInt()));
                        if(query1.value("status").toInt() == 0) newItem->setText("Finalized");
                        else newItem->setText("Canceled");
                    }
                    ui->invoiceTable->setItem(row, column, newItem);
                }
                row++;
            }
            //ui->invoiceTable->resizeColumnsToContents();
            ui->invoiceTable->horizontalHeader()->setHighlightSections(false);
            ui->invoiceTable->horizontalHeader()->setStyleSheet("QHeaderView::section { background-color:#0B2161;color:#fff; }");

        }
    }

}

/*
void Invoices::on_cancelInvoice_clicked()
{
    QMessageBox msgBox;
    DbMysql* d = DbMysql::getInstance();
    if(ui->invoiceTable->selectedItems().size() > 0)
    {
        int inv_id = ui->invoiceTable->selectedItems().at(0)->data(Qt::UserRole).toInt();
        if(!d->getConnection().open())
        {
            msgBox.critical(this,"Error","Failed to connect database.1");
        }
        else
        {

            QSqlQuery query1( "update invoice set status=1 where inv_id ="+QString::number(inv_id)+";",d->getConnection());
            qDebug()<<query1.lastQuery();
            if(!query1.isActive())
            {
                msgBox.critical(this,"Error","Failed to connect database.");
            }
            else
            {

                QSqlQuery query2( "delete from gst_data where inv_id ="+QString::number(inv_id)+";",d->getConnection());

                qDebug()<<query2.lastQuery();
                if(!query2.isActive())
                {
                    msgBox.critical(this,"Error","Failed to connect database.");
                }
                else
                {
                    msgBox.information(this,"Cancel Invoice","Invoice "+ui->invoiceTable->selectedItems().at(1)->text()+" canceled.");
                    this->createInvoiceTable();
                }
            }
        }
    }
    else
    {
        msgBox.warning(this,"Cancel Invoice","Please select a invoice from the table.");
        return ;
    }
}
*/

void Invoices::on_deleteInvoice_clicked(){
    QMessageBox msgBox;
    DbMysql* d = DbMysql::getInstance();
    if(ui->invoiceTable->selectedItems().size() > 0)
    {
        int inv_id = ui->invoiceTable->selectedItems().at(0)->data(Qt::UserRole).toInt();
        if(!d->getConnection().open())
        {
            msgBox.critical(this,"Error","Failed to connect database.1");
        }
        else
        {

            QSqlQuery query1( "delete from invoice where inv_id ="+QString::number(inv_id)+";",d->getConnection());
            qDebug()<<query1.lastQuery();
            if(!query1.isActive())
            {
                msgBox.critical(this,"Error","Failed to connect database.");
            }
            else
            {

                QSqlQuery query2( "delete from gst_data where inv_id ="+QString::number(inv_id)+";",d->getConnection());

                qDebug()<<query2.lastQuery();
                if(!query2.isActive())
                {
                    msgBox.critical(this,"Error","Failed to connect database.");
                }
                else
                {
                    msgBox.information(this,"Delete Invoice","Invoice "+ui->invoiceTable->selectedItems().at(1)->text()+" deleted.");
                    this->createInvoiceTable();
                }
            }
        }
    }
    else
    {
        msgBox.warning(this,"Delete Invoice","Please select a invoice from the table.");
        return ;
    }
}

int Invoices::findCustomerIdFormInvID(int inv_id)
{
    QMessageBox msgBox;
    DbMysql* d = DbMysql::getInstance();


    if(!d->getConnection().open())
    {
        msgBox.critical(this,"Error","Failed to connect database.1");
        return 0;
    }
    else
    {

        QSqlQuery query1( "SELECT bill.booking_id ,bill.invoice_id FROM invoice left join ( select booking_log.booking_id,room_billing.invoice_id from room_billing left join booking_log on booking_log.log_id = room_billing.logbook_id) as bill  on invoice.inv_id = bill.invoice_id where invoice_id  ="+QString::number(inv_id)+" limit 1;",d->getConnection());
        qDebug()<<query1.lastQuery();
        if(!query1.isActive())
        {
            msgBox.critical(this,"Error","Failed to connect database.");
            return 0;
        }
        else
        {
            query1.next();
            int booking_id = query1.value(0).toInt();
            QSqlQuery query2( "SELECT customer_id from booking where booking_id ="+QString::number(booking_id)+" limit 1;",d->getConnection());
            qDebug()<<query2.lastQuery();
            if(!query2.isActive())
            {
                msgBox.critical(this,"Error","Failed to connect database.");
                return 0;
            }
            else
            {
                query2.next();
                return  query2.value(0).toInt();
            }
        }
    }

}

void Invoices::on_printInvoice_clicked()
{
    QMessageBox msgBox;
    if(ui->invoiceTable->selectedItems().size() > 0)
    {
        int inv_id = ui->invoiceTable->selectedItems().at(0)->data(Qt::UserRole).toInt();
        int booking_id = ui->invoiceTable->item(ui->invoiceTable->selectedItems().at(0)->row(),9)->data(Qt::UserRole).toInt();


        QTextDocument *document = new QTextDocument();
        QString html = "<!DOCTYPE html><html><head><title>Invoice</title><meta http-equiv=\"Content-Type\" content =\"text/html;charset=utf-8\" ><style>table tr td { font-size:12px; }  table { border-width: 1px;border-style: solid;border-color: black;margin-top: 0px;margin-bottom: 0px;color: black;  } table td {padding: 5px;}</style></head>";
        html+= "<body style=\"color:#000;\" >";
        html+= "<h2 align=\"center\" >Invoice</h2><hr />";
        DbMysql* d = DbMysql::getInstance();


        if(!d->getConnection().open())
        {
            msgBox.critical(this,"Error","Failed to connect database.1");
        }
        else
        {

            QSqlQuery query( "SELECT  * from hotel_info;",d->getConnection());
            qDebug()<<query.lastQuery();
            if(!query.isActive())
            {
                msgBox.critical(this,"Error","Failed to connect database.");
            }
            else
            {
                query.next();
                html+= "<h1 align=\"center\" style='font-size:30px;font-weight:bold;' >"+query.value("hotel_name").toString()+"</h1>";
                html+= "<p align=\"center\" style='font-size:12px;font-weight:bold;' >"+query.value("address").toString()+"<br />"+query.value("dist").toString()+","+query.value("state").toString()+","+query.value("pin").toString()+"</p>";
                html+= "<p align=\"center\" style='font-size:12px;font-weight:bold;' ><b>Phone:</b>"+query.value("phone1").toString()+"/"+query.value("phone2").toString()+" <b>Email:</b> "+query.value("email").toString()+" <b>Website:</b> "+query.value("website").toString()+"</p>";

                html+="<p align=\"center\" style='font-size:14px;font-weight:bold;' ><b>GSTIN:</b> "+query.value("gst_number").toString()+"</p>";
                html+= "<hr style='border-bottom:5px solid #666' />";
            }
        }


        double room_tax=0;
        double food_tax = 0;

        QMap<QString,double> map1;



        if(!d->getConnection().open())
        {
            msgBox.critical(this,"Error","Failed to connect database.1");
        }
        else
        {

            QSqlQuery query1( "SELECT  * from invoice_details where inv_id ="+QString::number(inv_id)+";",d->getConnection());
            qDebug()<<query1.lastQuery();
            if(!query1.isActive())
            {
                msgBox.critical(this,"Error","Failed to connect database.");
            }
            else
            {
                query1.next();

                int booking_id = query1.value("booking_id").toInt();

                double cgst_total =0,sgst_total = 0;




                html+="<table width=\"100%\" cellspacing=\"0\" >";
                room_tax = query1.value("room_tax").toDouble();
                food_tax = query1.value("food_tax").toDouble();
                html+= "<tr><td align=\"left\" ><b>Invoice Number:</b> "+query1.value("inv_number").toString()+" </td><td align=\"right\" ><b>Invoice Date: </b>"+query1.value("inv_date").toDateTime().toString("dd-MM-yyyy HH:mm:ss")+" </td></tr>";
                html+= "</table><br />";
                html+= "<table cellspacing=\"0\" width=\"100%\" >";
                html+= "<tr><td colspan=\"2\" align=\"left\" ><b>Customer Name :</b> "+query1.value("customer_name").toString()+" </td></tr>";
                html+= "<tr><td colspan=\"2\" align=\"left\" ><b>Address : </b>"+query1.value("address").toString()+" </td></tr>";
                html+= "<tr><td align=\"left\" ><b>Phone : </b>"+query1.value("phone").toString()+" </td><td><b>Email: </b>"+query1.value("email").toString()+"</td></tr>";
                html+= "<tr><td align=\"left\" ><b>ID Type : </b>"+query1.value("id_type").toString()+" </td><td><b>ID Serial : </b>"+query1.value("id_serial").toString()+"</td></tr>";
                if(query1.value("gstin_no").toString().length() > 0) {
                    qDebug()<<"Subha";
                    html+= "<tr><td colspan='2' align=\"left\" ><b>Customer GSTIN: </b>"+query1.value("gstin_no").toString()+" </td></tr>";
                }
                html+= "<tr><td align=\"left\" ><b>Check In Time : </b>"+query1.value("in_time").toDateTime().toString("dd-MM-yyyy HH:mm:ss")+" </td><td><b>Check Out Time : </b>"+query1.value("out_time").toDateTime().toString("dd-MM-yyyy HH:mm:ss")+"</td></tr>";
                html+= "</table><br /><br />";


                html+= "<table width=\"100%\" cellspacing=\"0\"  font=10  >";
                html+= "<tr style='font-weight:bold;background:#efefef;' ><td>Room Number</td><td>Room Type</td><td>Booking From</td><td>Booking To</td><td>Days</td><td>Rate</td><td>Amount</td><td>Taxable Value</td><td>GST %</td><td>CGST</td><td>SGST</td><td>Total</td></tr>";

                double room_total_without_tax_amount = 0;
                double room_total_with_tax_amount = 0;
                QSqlQuery query3( " select * from room_booking_details where booking_id="+QString::number(booking_id)+";",d->getConnection());

                if(!query3.isActive())
                {
                    qDebug()<<query3.lastQuery();
                    msgBox.critical(this,"Error","Failed to connect database.");
                }
                else
                {
                    qDebug()<<query3.lastQuery();
                    while(query3.next())
                    {

                        /**GST CGST SGST Values Maps */

                        if(query3.value("cgst").toDouble()!= 0)
                        {
                            if(!map1.contains("cgst_"+query3.value("cgst").toString()))
                            {
                                map1.insert("cgst_"+query3.value("cgst").toString(),query3.value("cgst").toDouble());
                            }
                        }

                        html+= "<tr><td>"+query3.value("room_number").toString()+" </td>";
                        html+= "<td>"+query3.value("room_type").toString()+" </td>";
                        html+= "<td>"+query3.value("booking_from").toDate().toString("dd-MM-yyyy")+" </td>";
                        QString to = (query3.value("booking_to").toString() == query3.value("booking_from").toString())?" ":query3.value("booking_to").toDate().toString("dd-MM-yyyy");
                        html+= "<td>"+to+" </td>";
                        int days = query3.value("booking_from").toDate().daysTo(query3.value("booking_to").toDate()) + 1;
                        double room_total = query3.value("room_rate").toDouble()*days ;
                        double cgst_value = query3.value("cgst").toDouble()*room_total/100;
                        double sgst_value = query3.value("sgst").toDouble()*room_total/100;



                        html+= "<td>"+QString::number(days)+" </td>";
                        html+= "<td align='right' >"+QString::number(query3.value("room_rate").toDouble(),'f',2)+" </td>";
                        html+= "<td align='right' >"+QString::number(query3.value("room_rate").toDouble()*days,'f',2)+" </td>";
                        html+= "<td align='right' >"+QString::number(query3.value("room_rate").toDouble()*days,'f',2)+" </td>";
                        html+= "<td align='right' >"+QString::number(query3.value("cgst").toDouble()+query3.value("sgst").toDouble(),'f',2)+" </td>";
                        html+= "<td align='right' >"+QString::number(cgst_value,'f',2)+" </td>";
                        html+= "<td align='right' >"+QString::number(sgst_value,'f',2)+" </td>";

                        room_total_with_tax_amount+=room_total+cgst_value+sgst_value;
                        room_total_without_tax_amount+= room_total;

                        cgst_total+= cgst_value;
                        sgst_total+= sgst_value;
                        room_tax+= cgst_value+sgst_value;
                        html+= "<td width='200px' align='right' >"+QString::number(room_total+cgst_value+sgst_value,'f',2)+" </td>";
                        html+= "</tr>";

                    }
                }
                double total_advance = 0;
                QSqlQuery query4( " select * from advance_payments where booking_id="+QString::number(booking_id)+";",d->getConnection());

                if(!query4.isActive())
                {
                    qDebug()<<query4.lastQuery();
                    msgBox.critical(this,"Error","Failed to connect database.");
                }
                else
                {
                    while(query4.next())
                    {
                        total_advance+=query4.value("payment_value").toDouble();
                    }
                }

                QMapIterator<QString, double> i(map1);



                double room_tax_amount = room_total_with_tax_amount*room_tax/100;

                html+="<tr style='background-color:#eeeeee;font-weight:bold;' ><td colspan='6' align='right'  ><b>Total</b></td><td align='right'>"+QString::number(room_total_without_tax_amount,'f',2)+"</td><td align='right'>"+QString::number(room_total_without_tax_amount,'f',2)+"</td><td align='center' >-</td><td align='right'>"+QString::number(cgst_total,'f',2)+"</td><td align='right'>"+QString::number(sgst_total,'f',2)+"</td><td align='right'>"+QString::number(room_total_with_tax_amount,'f',2)+"</td></tr>";

                html+= "<tr><td colspan='6'  ><b>In words:</b> <b>"+AppUtils::numberToText(room_total_with_tax_amount)+"</b> </td><td colspan='5'><b>Total Amount</b></td><td align='right' >"+QString::number(room_total_without_tax_amount,'f',2)+"</td></tr>";
                html+= "<tr><td colspan='6' rowspan='5' ><p align='center' ><b><u>Tax Details</u></b></p>";
                QString cgst_html="";
                bool flag = true;
                while (i.hasNext()) {
                    i.next();
                    cgst_html+= "@ "+i.key().split("_").at(1)+" %";
                    if(i.hasNext()) cgst_html+=",";
                }

                html+="<p><b>CGST ("+cgst_html+") :</b>"+QString::number(cgst_total,'f',2)+"</p>";
                html+="<p><b>SGST ("+cgst_html+") :</b>"+QString::number(sgst_total,'f',2)+"</p>";


                html+="</td><td colspan='5' style='background-color:#eeeeee;font-weight:bold;' >Total Amount Before Tax</td><td align='right' style='background-color:#eeeeee;font-weight:bold;'>"+QString::number(room_total_without_tax_amount,'f',2)+"</td></tr>";

                html+= "<tr><td colspan='5' style='font-weight:bold;'>Add SGST</td><td align='right'>"+QString::number(sgst_total,'f',2)+"</td></tr>";
                html+= "<tr><td colspan='5' style='font-weight:bold;'>Add CGST</td><td align='right'>"+QString::number(cgst_total,'f',2)+"</td></tr>";
                html+= "<tr><td colspan='5' style='font-weight:bold;'>Tax Amount:  GST </td><td align='right'>"+QString::number(sgst_total+cgst_total,'f',2)+"</td></tr>";
                html+= "<tr><td colspan='5' style='background-color:#eeeeee;font-weight:bold;'>Grand Total </td><td align='right' style='background-color:#eeeeee;font-weight:bold;'>"+QString::number(room_total_with_tax_amount,'f',2)+" &#8377;</td></tr>";


                if(total_advance > 0)
                {
                    html+="<tr><td colspan='11' align='right'>Advance Paid </td><td align='right'>"+QString::number(total_advance,'f',2)+"</td></tr>";
                    html+="<tr style='background-color:#eeeeee;font-weight:bold;' ><td colspan='11' align='right'>Total Balance </td><td align='right'>"+QString::number(room_total_with_tax_amount-total_advance,'f',2)+" &#8377;</td></tr>";
                }
                html+= "</table>";


            }
        }

        html+="<br /><br />";
        html+="<table width='100%' border=0  cellspacing=\"0\" ><tr>";
        html+= "<td align='left' height=100 >Cashier Signature</td><td align='right' height=100 >Customer Signature</td></tr></table>";

        html+= "</body></html>";


        document->setHtml(html);

        QPrinter printer;

        QPrintDialog *dialog = new QPrintDialog(&printer, this);
        if (dialog->exec() != QDialog::Accepted)
            return;

        printer.setPageMargins(5,10,5,10,QPrinter::Millimeter);
        //printer.setFullPage(true);
        document->setPageSize(QSizeF(printer.pageRect().size()));

        document->print(&printer);

        delete document;
    }
}



void Invoices::on_printFoodInvoice_clicked()
{
    QMessageBox msgBox;
    if(ui->invoiceTable->selectedItems().size() > 0)
    {


        int inv_id = ui->invoiceTable->selectedItems().at(0)->data(Qt::UserRole).toInt();
        int booking_id = ui->invoiceTable->item(ui->invoiceTable->selectedItems().at(0)->row(),9)->data(Qt::UserRole).toInt();

        double room_tax=0;
        double food_tax = 0;

        QMap<QString,double> map1,map2;

        QTextDocument *document = new QTextDocument();
        QString html = "<!DOCTYPE html><html><head><title>Invoice</title><meta http-equiv=\"Content-Type\" content =\"text/html;charset=utf-8\" ><style>  table { border-width: 1px;border-style: solid;border-color: black;margin-top: 0px;margin-bottom: 0px;color: black;  } table td {padding: 2px;}</style></head>";
        html+= "<body style=\"color:#000;\" >";
        html+= "<h2 align=\"center\" >Food Invoice</h2><hr />";
        DbMysql* d = DbMysql::getInstance();

        if(!d->getConnection().open())
        {
            msgBox.critical(this,"Error","Failed to connect database.1");
        }
        else
        {

            QSqlQuery query( "SELECT  * from hotel_info;",d->getConnection());
            qDebug()<<query.lastQuery();
            if(!query.isActive())
            {
                msgBox.critical(this,"Error","Failed to connect database.");
            }
            else
            {
                query.next();
                html+= "<h1 align=\"center\" style='font-size:30px;font-weight:bold;' >"+query.value("hotel_name").toString()+"</h1>";
                html+= "<p align=\"center\" style='font-size:12px;font-weight:bold;' >"+query.value("address").toString()+"<br />"+query.value("dist").toString()+","+query.value("state").toString()+","+query.value("pin").toString()+"</p>";
                html+= "<p align=\"center\" style='font-size:12px;font-weight:bold;' ><b>Phone:</b>"+query.value("phone1").toString()+"/"+query.value("phone2").toString()+" <b>Email:</b> "+query.value("email").toString()+" <b>Website:</b> "+query.value("website").toString()+"</p>";

                html+="<p align=\"center\" style='font-size:14px;font-weight:bold;' ><b>GSTIN:</b> "+query.value("gst_number").toString()+"</p>";
                html+= "<hr style='border-bottom:5px solid #666' />";
            }
        }


        double food_total_amount = 0;


        if(!d->getConnection().open())
        {
            msgBox.critical(this,"Error","Failed to connect database.1");
        }
        else
        {

            QSqlQuery query1( "SELECT  * from invoice_details where inv_id ="+QString::number(inv_id)+";",d->getConnection());
            qDebug()<<query1.lastQuery();
            if(!query1.isActive())
            {
                msgBox.critical(this,"Error","Failed to connect database.");
            }
            else
            {
                query1.next();
                html+="<table width=\"100%\" cellspacing=\"0\" >";
                room_tax = query1.value("room_tax").toDouble();
                food_tax = query1.value("food_tax").toDouble();
                html+= "<tr><td align=\"left\" ><b>Invoice Number:</b> "+query1.value("inv_number").toString()+" </td><td align=\"right\" ><b>Invoice Date: </b>"+query1.value("inv_date").toDateTime().toString("dd-MM-yyyy HH:mm:ss")+" </td></tr>";
                html+= "</table><br />";
                html+= "<table width=\"100%\" cellspacing=\"0\"  >";
                html+= "<tr><td colspan=\"2\" align=\"left\" ><b>Customer Name :</b> "+query1.value("customer_name").toString()+" </td></tr>";
                html+= "<tr><td colspan=\"2\" align=\"left\" ><b>Address : </b>"+query1.value("address").toString()+" </td></tr>";
                html+= "<tr><td align=\"left\" ><b>Phone : </b>"+query1.value("phone").toString()+" </td><td><b>Email: </b>"+query1.value("email").toString()+"</td></tr>";
                html+= "<tr><td align=\"left\" ><b>ID Type : </b>"+query1.value("id_type").toString()+" </td><td><b>ID Serial : </b>"+query1.value("id_serial").toString()+"</td></tr>";
                if(query1.value("gstin_no").toString().length() > 0) {
                    qDebug()<<"Subha";
                    html+= "<tr><td colspan='2' align=\"left\" ><b>Customer GSTIN: </b>"+query1.value("gstin_no").toString()+" </td></tr>";
                }
                html+= "<tr><td align=\"left\" ><b>Check In Time : </b>"+query1.value("in_time").toDateTime().toString("dd-MM-yyyy HH:mm:ss")+" </td><td><b>Check Out Time : </b>"+query1.value("out_time").toDateTime().toString("dd-MM-yyyy HH:mm:ss")+"</td></tr>";
                html+= "</table><br /><br />";


                html+= "<table width=\"100%\" cellspacing=\"0\"  font=10 >";
                html+= "<tr style='font-weight:bold;background:#efefef;align:center;' ><td width='auto' >Item Name</td><td>Qty</td><td>Rate</td><td>Amount</td><td>GST(%)</td><td>CGST</td><td>SGST</td><td>Total</td></tr>";


                double room_total_amount = 0;
                double cgst_total = 0;
                double sgst_total = 0;



                QSqlQuery query3( " SELECT * FROM food_order_detailsfull where booking_id="+query1.value("booking_id").toString()+";",d->getConnection());
                qDebug()<<query3.lastQuery();

                double totalFoodAmountWithOutTax = 0;

                if(!query3.isActive())
                {
                    msgBox.critical(this,"Error","Failed to connect database.");
                }
                else
                {
                    while(query3.next())
                    {
                        html+= "<tr><td>"+query3.value("food_name").toString()+" </td>";
                        html+= "<td>"+query3.value("qty").toString()+" </td>";

                        double rate = query3.value("rate").toDouble();
                        double qty = query3.value("qty").toDouble();
                        double cgst = query3.value("cgst").toDouble();
                        double sgst = query3.value("sgst").toDouble();

                        if(cgst!= 0)
                        {
                            if(!map1.contains("cgst_"+QString::number(cgst)))
                            {
                                map1.insert("cgst_"+QString::number(cgst),cgst);
                            }
                        }

                        if(sgst!= 0)
                        {
                            if(!map2.contains("sgst_"+QString::number(sgst)))
                            {
                                map2.insert("sgst_"+QString::number(sgst),sgst);
                            }
                        }

                        html+= "<td align=\"right\" >"+QString::number(rate,'f',2)+" </td>";
                        html+= "<td align=\"right\" >"+QString::number(rate*qty,'f',2)+" </td>";

                        totalFoodAmountWithOutTax += rate*qty;

                        html+= "<td align=\"right\">"+QString::number(cgst+sgst,'f',2)+" % </td>";

                        html+= "<td align=\"right\">"+QString::number((cgst*rate*qty)/100,'f',2)+" </td>";
                        html+= "<td align=\"right\" >"+QString::number((sgst*rate*qty)/100,'f',2)+" </td>";

                        cgst_total+= (cgst*rate*qty)/100;
                        sgst_total+= (sgst*rate*qty)/100;

                        html+= "<td align='right' >"+QString::number(query3.value("total_amount").toDouble(),'f',2)+" </td>";
                        food_total_amount += query3.value("total_amount").toDouble();


                        html+= "</tr>";

                    }

                    double food_tax_amount = food_total_amount*food_tax/100;

                    html+="<tr style='border: none; background-color:#eeeeee;font-weight:bold;text-align:right;' ><td colspan='3' align='right' style='border: none;' >Total </td><td align='right'>"+QString::number(totalFoodAmountWithOutTax,'f',2)+"</td><td align='center' >-</td><td align='right'>"+QString::number(cgst_total,'f',2)+"</td><td align='right'>"+QString::number(sgst_total,'f',2)+"</td><td align='right'>"+QString::number(food_total_amount,'f',2)+"</td></tr>";



                    html+= "<tr><td colspan='3'  ><b>In words:</b> <b>"+AppUtils::numberToText(food_total_amount)+"</b> </td><td colspan='4' align='right'><b>Total Amount</b></td><td align='right' >"+QString::number(totalFoodAmountWithOutTax,'f',2)+"</td></tr>";
                    html+= "<tr><td colspan='3' rowspan='5' ><p align='center' ><b><u>Tax Details</u></b></p>";

                    QString cgst_html="";
                    QString sgst_html="";

                    QMapIterator<QString, double> i(map1);
                    QMapIterator<QString, double> j(map2);

                    bool flag = true;
                    while (i.hasNext()) {
                        i.next();
                        cgst_html+= "@ "+i.key().split("_").at(1)+" %";
                        if(i.hasNext()) cgst_html+=",";
                    }

                    while (j.hasNext()) {
                        j.next();
                        sgst_html+= "@ "+j.key().split("_").at(1)+" %";
                        if(j.hasNext()) sgst_html+=",";
                    }

                    html+="<p><b>CGST ("+cgst_html+") :</b> "+QString::number(cgst_total,'f',2)+" &#8377;</p>";
                    html+="<p><b>SGST ("+sgst_html+") :</b> "+QString::number(sgst_total,'f',2)+" &#8377;</p>";


                    html+="</td><td colspan='4' style='background-color:#eeeeee;font-weight:bold;' >Total Amount Before Tax</td><td align='right' style='background-color:#eeeeee;font-weight:bold;'>"+QString::number(totalFoodAmountWithOutTax,'f',2)+"</td></tr>";

                    html+= "<tr><td colspan='4' style='font-weight:bold;'>Add SGST</td><td align='right'>"+QString::number(sgst_total,'f',2)+"</td></tr>";
                    html+= "<tr><td colspan='4' style='font-weight:bold;'>Add CGST</td><td align='right'>"+QString::number(cgst_total,'f',2)+"</td></tr>";
                    html+= "<tr><td colspan='4' style='font-weight:bold;'>Tax Amount:  GST </td><td align='right'>"+QString::number(sgst_total+cgst_total,'f',2)+"</td></tr>";
                    html+= "<tr><td colspan='4' style='background-color:#eeeeee;font-weight:bold;'>Grand Total </td><td align='right' style='background-color:#eeeeee;font-weight:bold;'>"+QString::number(food_total_amount,'f',2)+" &#8377;</td></tr>";


                    /*
                    html+="<tr><td colspan='4' align='right' style='border: none;'>Service Tax ( "+QString::number(food_tax)+" % ) </td><td align='right'>"+QString::number(food_tax_amount,'f',2)+"</td></tr>";
                    html+="<tr><td colspan='4' align='right'>Grand Total </td><td align='right'>"+QString::number(food_tax_amount+food_total_amount,'f',2)+"</td></tr>";
                    */
                    html+= "</table>";

                }

            }
        }

        html+="<br /><br />";
        html+="<table width='100%' border=0 cellspacing='0' ><tr>";
        html+= "<td align='left' >Cashier Signature</td><td align='right' >Customer Signature</td></tr></table>";


        html+= "</body></html>";

        document->setHtml(html);

        QPrinter printer;

        QPrintDialog *dialog = new QPrintDialog(&printer, this);
        if (dialog->exec() != QDialog::Accepted)
            return;

        printer.setPageMargins(5,10,5,10,QPrinter::Millimeter);
        //printer.setFullPage(true);
        document->setPageSize(QSizeF(printer.pageRect().size()));

        document->print(&printer);

        delete document;
    }
}

/*

QString Invoices::getLodgingInvoice()
{
    QMessageBox msgBox;
    if(ui->invoiceTable->selectedItems().size() > 0)
    {
        int inv_id = ui->invoiceTable->selectedItems().at(0)->data(Qt::UserRole).toInt();
        int booking_id = ui->invoiceTable->item(ui->invoiceTable->selectedItems().at(0)->row(),9)->data(Qt::UserRole).toInt();

        double room_tax=0;
        double food_tax = 0;



        QString html = "<!DOCTYPE html><html><head><title>Invoice</title><meta http-equiv=\"Content-Type\" content =\"text/html;charset=utf-8\" ><style type=\"text/css\" >table tr td { font-size:8px; } table { }</style></head>";
        html+= "<body style=\"color:#000;\" >";
        DbMysql* d = DbMysql::getInstance();



        if(!d->getConnection().open())
        {
            msgBox.critical(this,"Error","Failed to connect database.1");
        }
        else
        {

            QSqlQuery query1( "SELECT  * from invoice_details where inv_id ="+QString::number(inv_id)+";",d->getConnection());
            qDebug()<<query1.lastQuery();
            if(!query1.isActive())
            {
                msgBox.critical(this,"Error","Failed to connect database.");
            }
            else
            {
                query1.next();
                html+="<table border=1 width='100%'>";
                room_tax = query1.value("room_tax").toDouble();
                food_tax = query1.value("food_tax").toDouble();
                html+= "<tr><td align=\"left\" ><b>Invoice Number:</b> "+query1.value("inv_number").toString()+" </td><td align=\"right\" ><b>Invoice Date: </b>"+query1.value("inv_date").toDateTime().toString("dd-MM-yyyy HH:mm:ss")+" </td></tr>";
                html+= "</table><br />";
                html+= "<table cellpadding=\"5\" border=1 width=\"100%\" >";
                html+= "<tr><td colspan=\"2\" align=\"left\" ><b>Customer Name :</b> "+query1.value("customer_name").toString()+" </td></tr>";
                html+= "<tr><td colspan=\"2\" align=\"left\" ><b>Address : </b>"+query1.value("address").toString()+" </td></tr>";
                html+= "<tr><td align=\"left\" ><b>Phone : </b>"+query1.value("phone").toString()+" </td><td><b>Email: </b>"+query1.value("email").toString()+"</td></tr>";
                html+= "<tr><td align=\"left\" ><b>ID Type : </b>"+query1.value("id_type").toString()+" </td><td><b>ID Serial : </b>"+query1.value("id_serial").toString()+"</td></tr>";
                html+= "<tr><td align=\"left\" ><b>Check In Time : </b>"+query1.value("in_time").toDateTime().toString("dd-MM-yyyy HH:mm:ss")+" </td><td><b>Check Out Time : </b>"+query1.value("out_time").toDateTime().toString("dd-MM-yyyy HH:mm:ss")+"</td></tr>";
                html+= "</table><br /><br />";


                html+= "<table cellpadding=\"5\" border=1 cellspacing=1 font=10 width=\"100%\" >";
                html+= "<tr style='font-weight:bold;background:#efefef;' ><td>Room Number</td><td>Room Type</td><td>Booking From</td><td>Booking To</td><td>Days</td><td>Rate</td><td>GST %</td><td>CGST</td><td>SGST</td><td>Amount</td></tr>";
                double room_total_amount = 0;
                QSqlQuery query3( " select * from room_booking_details where booking_id="+QString::number(booking_id)+";",d->getConnection());

                if(!query3.isActive())
                {
                    qDebug()<<query3.lastQuery();
                    msgBox.critical(this,"Error","Failed to connect database.");
                }
                else
                {
                    qDebug()<<query3.lastQuery();
                    while(query3.next())
                    {
                        html+= "<tr><td>"+query3.value("room_number").toString()+" </td>";
                        html+= "<td>"+query3.value("room_type").toString()+" </td>";
                        html+= "<td>"+query3.value("booking_from").toDate().toString("dd-MM-yyyy")+" </td>";
                        QString to = (query3.value("booking_to").toString() == query3.value("booking_from").toString())?" ":query3.value("booking_to").toDate().toString("dd-MM-yyyy");
                        html+= "<td>"+to+" </td>";

                        double room_total = query3.value("room_rate").toDouble()*((query3.value("booking_from").toDate().daysTo(query3.value("booking_to").toDate()))+1) ;

                        html+= "<td>"+QString::number(((query3.value("booking_from").toDate().daysTo(query3.value("booking_to").toDate()))+1))+" </td>";
                        html+= "<td align='right' >"+QString::number(query3.value("room_rate").toDouble(),'f',2)+" </td>";
                        html+= "<td align='right' >"+QString::number(query3.value("cgst").toDouble()+query3.value("sgst").toDouble(),'f',2)+" </td>";
                        html+= "<td align='right' >"+QString::number(query3.value("cgst").toDouble()*room_total/100,'f',2)+" </td>";
                        html+= "<td align='right' >"+QString::number(query3.value("sgst").toDouble()*room_total/100,'f',2)+" </td>";

                        room_total_amount+=room_total;
                        html+= "<td width='200px' align='right' >"+QString::number(room_total,'f',2)+" </td>";
                        html+= "</tr>";

                    }
                }
                double total_advance = 0;
                QSqlQuery query4( " select * from advance_payments where booking_id="+QString::number(booking_id)+";",d->getConnection());

                if(!query4.isActive())
                {
                    qDebug()<<query4.lastQuery();
                    msgBox.critical(this,"Error","Failed to connect database.");
                }
                else
                {
                    while(query4.next())
                    {
                        total_advance+=query4.value("payment_value").toDouble();
                    }
                }

                double room_tax_amount = room_total_amount*room_tax/100;

                html+="<tr style='border: none;' ><td colspan='6' align='right' style='border: none;' >Total Amount </td><td align='right'>"+QString::number(room_total_amount,'f',2)+"</td></tr>";
                html+="<tr><td colspan='6' align='right' style='border: none;'>Service Tax ( "+QString::number(room_tax)+" % ) </td><td align='right'>"+QString::number(room_tax_amount,'f',2)+"</td></tr>";
                html+="<tr><td colspan='6' align='right'>Grand Total </td><td align='right'>"+QString::number(room_tax_amount+room_total_amount,'f',2)+"</td></tr>";
                if(total_advance > 0)
                {
                    html+="<tr><td colspan='6' align='right'>Advance Paid </td><td align='right'>"+QString::number(total_advance,'f',2)+"</td></tr>";
                    html+="<tr><td colspan='6' align='right'>Total Balance </td><td align='right'>"+QString::number(room_tax_amount+room_total_amount-total_advance,'f',2)+"</td></tr>";
                }
                html+= "</table>";
                html+= "<p>(<b>In words:</b> "+AppUtils::numberToText(room_tax_amount+room_total_amount-total_advance)+" )</p>";

            }
        }

        html+="<br /><br />";
        html+="<table width='100%' ><tr>";
        html+= "<td align='left' >Cashier Signature</td><td align='right' >Customer Signature</td></tr></table>";

        html+= "</body></html>";

        return html;

    }
    return "";
}



QString Invoices::getFoodInvoice()
{
    QMessageBox msgBox;
    if(ui->invoiceTable->selectedItems().size() > 0)
    {


        int inv_id = ui->invoiceTable->selectedItems().at(0)->data(Qt::UserRole).toInt();
        int booking_id = ui->invoiceTable->item(ui->invoiceTable->selectedItems().at(0)->row(),9)->data(Qt::UserRole).toInt();

        double room_tax=0;
        double food_tax = 0;

        DbMysql* d = DbMysql::getInstance();

        QString html = "<!DOCTYPE html><html><head><title>Invoice</title><meta http-equiv=\"Content-Type\" content =\"text/html;charset=utf-8\" ><style type=\"text/css\" >table tr td { font-size:12px; } \n table { border-collapse: collapse; }</style></head>";
        html+= "<body style=\"color:#000;\" >";


        double food_total_amount = 0;


        if(!d->getConnection().open())
        {
            msgBox.critical(this,"Error","Failed to connect database.1");
        }
        else
        {

            QSqlQuery query1( "SELECT  * from invoice_details where inv_id ="+QString::number(inv_id)+";",d->getConnection());
            qDebug()<<query1.lastQuery();
            if(!query1.isActive())
            {
                msgBox.critical(this,"Error","Failed to connect database.");
            }
            else
            {
                query1.next();
                html+="<table width='100%'>";
                room_tax = query1.value("room_tax").toDouble();
                food_tax = query1.value("food_tax").toDouble();
                html+= "<tr><td align=\"left\" ><b>Invoice Number:</b> "+query1.value("inv_number").toString()+" </td><td align=\"right\" ><b>Invoice Date: </b>"+query1.value("inv_date").toDateTime().toString("dd-MM-yyyy HH:mm:ss")+" </td></tr>";
                html+= "</table><br />";
                html+= "<table cellpadding=\"5\" border=1 cellspacing=1  width=\"100%\" >";
                html+= "<tr><td colspan=\"2\" align=\"left\" ><b>Customer Name :</b> "+query1.value("customer_name").toString()+" </td></tr>";
                html+= "<tr><td colspan=\"2\" align=\"left\" ><b>Address : </b>"+query1.value("address").toString()+" </td></tr>";

                html+= "</table><br /><br />";


                html+= "<table cellpadding=\"5\" border=1 cellspacing=1  width=\"100%\" >";
                html+= "<tr style='font-weight:bold;background:#efefef;' ><td>Item Name</td><td>Order Time</td><td>Qty</td><td>Rate</td><td>Amount</td></tr>";


                double room_total_amount = 0;
                QSqlQuery query3( " SELECT food_name,qty,order_time,rate,(rate*qty) as amount FROM food_orders where booking_id="+QString::number(booking_id)+";",d->getConnection());
                qDebug()<<query3.lastQuery();
                if(!query3.isActive())
                {
                    msgBox.critical(this,"Error","Failed to connect database.");
                }
                else
                {
                    while(query3.next())
                    {
                        html+= "<tr><td>"+query3.value("food_name").toString()+" </td>";
                        html+= "<td>"+query3.value("order_time").toDateTime().toString("dd-MM-yyyy HH:mm:ss")+" </td>";
                        html+= "<td>"+query3.value("qty").toString()+" </td>";
                        html+= "<td>"+QString::number(query3.value("rate").toDouble(),'f',2)+" </td>";
                        html+= "<td align='right' >"+QString::number(query3.value("amount").toDouble(),'f',2)+" </td>";
                        food_total_amount += query3.value("amount").toDouble();


                        html+= "</tr>";

                    }

                    double food_tax_amount = food_total_amount*food_tax/100;

                    html+="<tr style='border: none;' ><td colspan='4' align='right' style='border: none;' >Total Amount </td><td align='right'>"+QString::number(food_total_amount,'f',2)+"</td></tr>";
                    html+="<tr><td colspan='4' align='right' style='border: none;'>Service Tax ( "+QString::number(food_tax)+" % ) </td><td align='right'>"+QString::number(food_tax_amount,'f',2)+"</td></tr>";
                    html+="<tr><td colspan='4' align='right'>Grand Total </td><td align='right'>"+QString::number(food_tax_amount+food_total_amount,'f',2)+"</td></tr>";
                    html+= "</table>";
                    html+= "<p>(<b>In words:</b> "+AppUtils::numberToText(food_tax_amount+food_total_amount)+" )</p>";
                }

            }
        }

        html+="<br /><br />";
        html+="<table width='100%' ><tr>";
        html+= "<td align='left' >Cashier Signature</td><td align='right' >Customer Signature</td></tr></table>";


        html+= "</body></html>";


        return html;
    }
    return "";
}
*/


void Invoices::on_paymentDoneBtn_clicked()
{
    QMessageBox msgBox;
    if(ui->invoiceTable->selectedItems().size() > 0)
    {
        int booking_id = ui->invoiceTable->item(ui->invoiceTable->selectedItems().at(0)->row(),8)->data(Qt::UserRole).toInt();
        DbMysql* d = DbMysql::getInstance();
        if(!d->getConnection().open())
        {
            msgBox.critical(this,"Error","Failed to connect database.1");
        }
        else
        {

            QSqlQuery query1( "update booking set payment_status=1 where booking_id ="+QString::number(booking_id)+";",d->getConnection());
            qDebug()<<query1.lastQuery();
            if(!query1.isActive())
            {
                msgBox.critical(this,"Error","Failed to connect database.");
            }
            else
            {
                 msgBox.information(this,"Make Payment","Payment done for booking.");
            }
        }
    }
    else
    {
        msgBox.information(this,"Make Payment","Please select a invoice for payment");
    }
    this->createInvoiceTable();
}

void Invoices::on_invoiceTable_cellDoubleClicked(int row, int column)
{
    /*
    ui->lBill->setVisible(true);
    ui->fBill->setVisible(true);
    ui->lBill->setText(getLodgingInvoice());
    ui->fBill->setText(getFoodInvoice());
    */

    InvoiceViewer *iv = new InvoiceViewer();
    iv->getInvoices(ui->invoiceTable->selectedItems().at(0)->data(Qt::UserRole).toInt());
    iv->show();


}

void Invoices::on_filterBtn_clicked()
{
    this->createInvoiceTable();
}

void Invoices::on_pushButton_clicked()
{
    DbMysql* d = DbMysql::getInstance();
    QMessageBox msgBox;

    if(!d->getConnection().open())
    {
        msgBox.critical(this,"Error","Failed to connect database.1");
    }
    else
    {

        QSqlQuery query1( "SELECT  inv_id,inv_number,inv_date,customer_name,room_numbers,lodging_bill_total,fooding_bill_total,ps_type,status,booking_id from invoice_details inner join payment_status on payment_status.ps_id = invoice_details.payment_status where inv_number like '%"+ui->invNum->text()+"%' order by inv_id desc",d->getConnection());
        qDebug()<<query1.lastQuery();
        if(!query1.isActive())
        {
            msgBox.critical(this,"Error","Failed to connect database.");
        }
        else
        {
            ui->invoiceTable->setRowCount(query1.size());
            int row = 0 ;

            while(query1.next())
            {

                int columnSize = ui->invoiceTable->columnCount();
                qDebug()<<"Column Size:"<<columnSize;



                for( int column = 0; column < columnSize; column++)
                {
                    QTableWidgetItem *newItem = new QTableWidgetItem();
                    newItem->setData(Qt::UserRole,QVariant::fromValue(query1.value(0).toInt()));

                    //newItem->setData(Qt::,QVariant::fromValue(query1.value(columnSize).toInt()));
                    newItem->setText(query1.value(column).toString());

                    if(query1.value("status").toInt() == 1)
                    {
                        QColor clr;
                        clr.setNamedColor("#FFB3AD");
                        newItem->setBackgroundColor(clr);
                    }

                    if(column==8)
                    {
                        newItem->setData(Qt::UserRole,QVariant::fromValue(query1.value(9).toInt()));
                        if(query1.value("status").toInt() == 0) newItem->setText("Finalized");
                        else newItem->setText("Canceled");
                    }
                    ui->invoiceTable->setItem(row, column, newItem);
                }



                row++;
            }
            ui->invoiceTable->resizeColumnsToContents();
            ui->invoiceTable->horizontalHeader()->setHighlightSections(false);
            ui->invoiceTable->horizontalHeader()->setStyleSheet("QHeaderView::section { background-color:#0B2161;color:#fff; }");

        }
    }
}
