#include "invoiceviewer.h"
#include "ui_invoiceviewer.h"
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
#include <QMapIterator>


InvoiceViewer::InvoiceViewer(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::InvoiceViewer)
{
    ui->setupUi(this);
}

void InvoiceViewer::getInvoices(int aInvId)
{
    ui->lodgInv->setText(getLodgingInvoice(aInvId));
    ui->foodInv->setText(getFoodingInvoice(aInvId));
}


InvoiceViewer::~InvoiceViewer()
{
    delete ui;
}


QString InvoiceViewer::getLodgingInvoice(int aInvId)
{
    QMessageBox msgBox;

    int inv_id = aInvId;
    //int booking_id = ui->invoiceTable->item(ui->invoiceTable->selectedItems().at(0)->row(),9)->data(Qt::UserRole).toInt();

    double room_tax=0;
    double food_tax = 0;

    QString html = "<!DOCTYPE html><html><head><title>Invoice</title><meta http-equiv=\"Content-Type\" content =\"text/html;charset=utf-8\" ><style>table tr td { font-size:12px; }  table { border-width: 1px;border-style: solid;border-color: black;margin-top: 0px;margin-bottom: 0px;color: black;  } table td {padding: 5px;} </style></head>";
    html+= "<body style=\"color:#000;\" >";
    DbMysql* d = DbMysql::getInstance();

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




            html+="<table width=\"100%\" cellspacing=\"0\">";
            room_tax = query1.value("room_tax").toDouble();
            food_tax = query1.value("food_tax").toDouble();
            html+= "<tr><td align=\"left\" ><b>Invoice Number:</b> "+query1.value("inv_number").toString()+" </td><td align=\"right\" ><b>Invoice Date: </b>"+query1.value("inv_date").toDateTime().toString("dd-MM-yyyy HH:mm:ss")+" </td></tr>";
            html+= "</table><br />";
            html+= "<table width=\"100%\" cellspacing=\"0\" width=\"100%\" >";
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


            html+= "<table width=\"100%\" cellspacing=\"0\" font=10 >";
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
    html+="<table width='100%' ><tr>";
    html+= "<td align='left' >Cashier Signature</td><td align='right' >Customer Signature</td></tr></table>";

    html+= "</body></html>";

    return html;

}


QString InvoiceViewer::getFoodingInvoice(int aInvId)
{
    QMessageBox msgBox;



    int inv_id = aInvId;


    double room_tax=0;
    double food_tax = 0;

    DbMysql* d = DbMysql::getInstance();
    QMap<QString,double> map1,map2;

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
            html+= "<tr><td align=\"left\" ><b>Phone : </b>"+query1.value("phone").toString()+" </td><td><b>Email: </b>"+query1.value("email").toString()+"</td></tr>";
            html+= "<tr><td align=\"left\" ><b>ID Type : </b>"+query1.value("id_type").toString()+" </td><td><b>ID Serial : </b>"+query1.value("id_serial").toString()+"</td></tr>";
            if(query1.value("gstin_no").toString().length() > 0) {
                qDebug()<<"Subha";
                html+= "<tr><td colspan='2' align=\"left\" ><b>Customer GSTIN: </b>"+query1.value("gstin_no").toString()+" </td></tr>";
            }
            html+= "<tr><td align=\"left\" ><b>Check In Time : </b>"+query1.value("in_time").toDateTime().toString("dd-MM-yyyy HH:mm:ss")+" </td><td><b>Check Out Time : </b>"+query1.value("out_time").toDateTime().toString("dd-MM-yyyy HH:mm:ss")+"</td></tr>";
            html+= "</table><br /><br />";


            html+= "<table cellpadding=\"5\" border=1 cellspacing=1  width=\"100%\" >";
            html+= "<tr style='font-weight:bold;background:#efefef;' ><td>Item Name</td><td>Qty</td><td>Rate</td><td>Amount</td><td>GST(%)</td><td>CGST</td><td>SGST</td><td>Total</td></tr>";


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
