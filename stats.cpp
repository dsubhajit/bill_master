#include "stats.h"
#include "ui_stats.h"
#include "qcustomplot.h"
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

Stats::Stats(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Stats)
{
    ui->setupUi(this);
    QStringList months;
    //ui->expTime->setDateTime(QDateTime::currentDateTime());

    months<<"Jan"<<"Feb"<<"Mar"<<"Apr"<<"May"<<"Jun"<<"Jul"<<"Aug"<<"Sep"<<"Oct"<<"Nov"<<"Dec";
    int i=1;
    int cur_mnth = QDate::currentDate().month();

    foreach (QString mnth, months) {
        ui->month->addItem(mnth,QVariant::fromValue(i++));

    }
    ui->month->setCurrentIndex(cur_mnth-1);

    int cur_year= QDate::currentDate().year();

    for(int i=2000;i<2050;i++)
    {
        ui->year->addItem(QString::number(i),QVariant::fromValue(i));
    }
    ui->year->setCurrentIndex(cur_year-2000);

    this->createGraph();
    this->createBookingTable();
    this->createInvoiceTable();
}

Stats::~Stats()
{
    delete ui;
}

void Stats::createGraph()
{
    int year = ui->year->currentData(Qt::UserRole).toInt();


    for(int i=0;i<ui->customPlot->graphCount();i++)
    {
        ui->customPlot->removeGraph(ui->customPlot->graph(i));
    }

    for(int j=0;j<ui->customPlot->plottableCount();j++)
    {
        ui->customPlot->removePlottable(ui->customPlot->plottable(j));
    }

    QCPBars *myBars = new QCPBars(ui->customPlot->xAxis, ui->customPlot->yAxis);
    ui->customPlot->addPlottable(myBars);


    QVector<double> keyData;
    QVector<double> valueData;

    QVector<double> ticks;
    QVector<QString> labels;

    labels << "Jan"<<"Feb"<<"Mar"<<"Apr"<<"May"<<"Jun"<<"Jul"<<"Aug"<<"Sep"<<"Oct"<<"Nov"<<"Dec";

    DbMysql* d = DbMysql::getInstance();
    QMessageBox msgBox;

    int max = 0;

    if(!d->getConnection().open())
    {
        msgBox.critical(this,"Error","Failed to connect database.1");
        return ;
    }
    else
    {

        QString curDate = QDate::currentDate().toString("yyyy-MM-dd");

        QSqlQuery query1( "SELECT  m, Count(booking.booking_from) as count FROM (SELECT y, m FROM (SELECT "+QString::number(year)+" y ) years, (SELECT 1 m UNION ALL SELECT 2 UNION ALL SELECT 3 UNION ALL SELECT 4 UNION ALL SELECT 5 UNION ALL SELECT 6 UNION ALL SELECT 7 UNION ALL SELECT 8 UNION ALL SELECT 9 UNION ALL SELECT 10 UNION ALL SELECT 11 UNION ALL SELECT 12) months ) ym  LEFT JOIN booking on ym.y = YEAR(booking.booking_from) and ym.m = MONTH(booking.booking_from)  group by m ;" ,d->getConnection());
        if(!query1.isActive())
        {
            msgBox.critical(this,"Error","Failed to connect database.");
            qDebug()<<query1.lastQuery();
            return ;
        }
        else
        {
            qDebug()<<query1.lastQuery();
            while(query1.next())
            {
                valueData <<query1.value(1).toDouble();
                keyData<<query1.value(0).toDouble();
                if(query1.value(1).toInt() > max) max =query1.value(1).toInt();
                ticks<<query1.value(0).toDouble();
            }
        }
    }

    QPen pen;
    pen.setWidthF(1.2);
    myBars->setName("Booking Chart");
    pen.setColor(QColor(150, 222, 0));
    myBars->setPen(pen);
    myBars->setBrush(QColor(150, 222, 0, 70));

    ui->customPlot->xAxis->setAutoTicks(false);
    ui->customPlot->xAxis->setAutoTickLabels(false);
    ui->customPlot->xAxis->setTickVector(ticks);
    ui->customPlot->xAxis->setTickVectorLabels(labels);
    ui->customPlot->xAxis->setTickLabelRotation(60);
    ui->customPlot->xAxis->setSubTickCount(0);
    ui->customPlot->xAxis->setTickLength(0, 4);
    ui->customPlot->xAxis->grid()->setVisible(true);
    ui->customPlot->xAxis->setRange(0, 12);

    ui->customPlot->yAxis->setRange(0, max);
    ui->customPlot->yAxis->setPadding(5);
    ui->customPlot->yAxis->setLabel("No of bookings per month");
    ui->customPlot->yAxis->grid()->setSubGridVisible(true);
    QPen gridPen;
    gridPen.setStyle(Qt::SolidLine);
    gridPen.setColor(QColor(0, 0, 0, 25));
    ui->customPlot->yAxis->grid()->setPen(gridPen);
    gridPen.setStyle(Qt::DotLine);
    ui->customPlot->yAxis->grid()->setSubGridPen(gridPen);
    /*
    ui->customPlot->legend->setVisible(true);
    ui->customPlot->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignTop|Qt::AlignHCenter);
    ui->customPlot->legend->setBrush(QColor(255, 255, 255, 200));
    QPen legendPen;
    legendPen.setColor(QColor(130, 130, 130, 200));
    ui->customPlot->legend->setBorderPen(legendPen);
    QFont legendFont = font();
    legendFont.setPointSize(10);
    ui->customPlot->legend->setFont(legendFont);
    ui->customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
    */
    myBars->setData(keyData, valueData);
    ui->customPlot->rescaleAxes();
    ui->customPlot->replot();
}

void Stats::createBookingTable()
{
    int year = ui->year->currentData(Qt::UserRole).toInt();
    int month = ui->month->currentData(Qt::UserRole).toInt();

    DbMysql* d = DbMysql::getInstance();
    QMessageBox msgBox;

    if(!d->getConnection().open())
    {
        msgBox.critical(this,"Error","Failed to connect database.1");
    }
    else
    {

        QSqlQuery query1( "select booking_id,booking_date,customer_name,room_numbers,booking_from,booking_to,reg_serial,in_time,out_time,nop from booking_details where MONTH(booking_from) ="+QString::number(month)+" and  YEAR(booking_from)="+QString::number(year)+" order by booking_id desc;" ,d->getConnection());
        if(!query1.isActive())
        {
            qDebug()<<query1.lastQuery();
            msgBox.critical(this,"Error","Failed to connect database.");
        }
        else
        {
            ui->bookingTable->setRowCount(query1.size());
            int row = 0 ;

            while(query1.next())
            {
                int columnSize = ui->bookingTable->columnCount();
                qDebug()<<"Column Size:"<<columnSize;

                for( int column = 0; column < columnSize-1; column++)
                {
                    QTableWidgetItem *newItem = new QTableWidgetItem();
                    newItem->setData(Qt::UserRole,QVariant::fromValue(query1.value(0).toInt()));
                    newItem->setText(query1.value(column).toString());
                    ui->bookingTable->setItem(row, column, newItem);

                }

                QDate frm_Date = QDate::fromString(query1.value(4).toString(),"yyyy-MM-dd");
                QDate to_Date = QDate::fromString(query1.value(5).toString(),"yyyy-MM-dd");



                int numDays = frm_Date.daysTo(to_Date)+1;

                qDebug()<<numDays<<" "<<frm_Date<<" "<<to_Date;

                QTableWidgetItem *newItem = new QTableWidgetItem();
                newItem->setData(Qt::UserRole,QVariant::fromValue(query1.value(0).toInt()));
                newItem->setText(QString::number(numDays));
                ui->bookingTable->setItem(row,columnSize-1, newItem);

                row++;
            }
            ui->bookingTable->resizeColumnsToContents();
            ui->bookingTable->horizontalHeader()->setHighlightSections(false);
            //ui->bookingLogTable->horizontalHeader()->setStyleSheet("QHeaderView::section { background-color:#0B2161;color:#fff; }");
        }
    }
}

void Stats::on_filterBtn_clicked()
{
    this->createGraph();
    this->createBookingTable();
    this->createInvoiceTable();
}

void Stats::on_printBookingList_clicked()
{
    QMessageBox msgBox;
    DbMysql* d = DbMysql::getInstance();
    int year = ui->year->currentData(Qt::UserRole).toInt();
    int month = ui->month->currentData(Qt::UserRole).toInt();

    QTextDocument *document = new QTextDocument();
    QString html = "<!DOCTYPE html><html><head><title>Invoice</title><meta http-equiv=\"Content-Type\" content =\"text/html;charset=utf-8\" ><style type=\"text/css\" >table tr td { font-size:12px; } table { border-collapse: collapse; }</style></head>";
    html+= "<body style=\"color:#000;width:1024px;\" >";
    html+="<h1 align='center' >Hotel Purulia Inn</h1>";
    html+= "<h4 align='center'>Booking List</h4><br />";
    QDate dt(year,month,1);

    html+= "<p>For "+dt.toString("MMMM yyyy")+"</p>";
    html+= "<hr />";
    html+= "<table width='100%' border=1 cellpadding='3' >";
    html+= "<tr style='background:#efefef;'><td>#</td><td>Booking Date</td><td>Customer Name</td><td>Room Numbers</td><td>Booking From</td><td>Booking To</td><td>Reg Serial</td><td>No.Of Persons</td></tr>";


    if(!d->getConnection().open())
    {
        msgBox.critical(this,"Error","Failed to connect database.1");
    }
    else
    {

        QSqlQuery query1( "select booking_id,booking_date,customer_name,room_numbers,booking_from,booking_to,reg_serial,in_time,out_time,nop from booking_details where MONTH(booking_from) ="+QString::number(month)+" and  YEAR(booking_from)="+QString::number(year)+" order by booking_id desc;" ,d->getConnection());
        if(!query1.isActive())
        {
            qDebug()<<query1.lastQuery();
            msgBox.critical(this,"Error","Failed to connect database.");
        }
        else
        {
            while(query1.next())
            {
                html+= "<tr>";
                int columnSize = ui->bookingTable->columnCount();
                for( int column = 0; column < columnSize; column++)
                {
                    html+="<td>"+query1.value(column).toString()+"</td>";
                }
                html+= "</tr>";
            }
        }
    }

    html+="</table>";

    html+= "</body></html>";


    document->setHtml(html);

    QPrinter printer;

    QPrintDialog *dialog = new QPrintDialog(&printer, this);
    if (dialog->exec() != QDialog::Accepted)
        return;

    printer.setPageMargins(5,10,5,10,QPrinter::Millimeter);
    //printer.setFullPage(true);
    document->setPageSize(QSizeF(printer.pageRect().size()));

    printer.setResolution (400);

    //QWebView* wv = new QWebView();
    //wv->setHtml(html);

    //wv->print(&printer);
    //delete wv;
    document->print(&printer);

    delete document;

}

void Stats::on_printCustomerList_clicked()
{
    QMessageBox msgBox;
    DbMysql* d = DbMysql::getInstance();
    int year = ui->year->currentData(Qt::UserRole).toInt();
    int month = ui->month->currentData(Qt::UserRole).toInt();

    QTextDocument *document = new QTextDocument();
    QString html = "<!DOCTYPE html><html><head><title>Invoice</title><meta http-equiv=\"Content-Type\" content =\"text/html;charset=utf-8\" ><style type=\"text/css\" >table tr td { font-size:12px; } table { border-collapse: collapse; }</style></head>";
    html+= "<body style=\"color:#000;width:1024px;\" >";
    html+="<h1 align='center' >Hotel Purulia Inn</h1>";
    html+= "<h4 align='center'>Customer List</h4><br />";
    QDate dt(year,month,1);

    html+= "<p>For "+dt.toString("MMMM yyyy")+"</p>";
    html+= "<hr />";
    html+= "<table width='100%' border=1 cellpadding='3' >";
    html+= "<tr style='background:#efefef;border:1px;border-color:#000;' ><td>Customer Name</td><td>Address</td><td>Email</td><td>Phone</td><td>ID Type</td><td>ID Serial</td><td>Room Numbers</td><td>Booking From</td><td>Booking To</td>";


    if(!d->getConnection().open())
    {
        msgBox.critical(this,"Error","Failed to connect database.1");
    }
    else
    {

        QSqlQuery query1( "select customer_name,address,email,phone,id_type,id_serial,room_numbers,booking_from,booking_to from booking_details where MONTH(booking_from) ="+QString::number(month)+" and  YEAR(booking_from)="+QString::number(year)+" order by booking_id desc;" ,d->getConnection());
        if(!query1.isActive())
        {
            qDebug()<<query1.lastQuery();
            msgBox.critical(this,"Error","Failed to connect database.");
        }
        else
        {
            while(query1.next())
            {
                html+= "<tr>";
                int columnSize = 9;
                for( int column = 0; column < columnSize; column++)
                {
                    html+="<td>"+query1.value(column).toString()+"</td>";
                }
                html+= "</tr>";
            }
        }
    }

    html+="</table>";

    html+= "</body></html>";


    document->setHtml(html);

    QPrinter printer;

    QPrintDialog *dialog = new QPrintDialog(&printer, this);
    if (dialog->exec() != QDialog::Accepted)
        return;

    printer.setPageMargins(5,10,5,10,QPrinter::Millimeter);
    //printer.setFullPage(true);
    document->setPageSize(QSizeF(printer.pageRect().size()));

    printer.setResolution (400);

    //QWebView* wv = new QWebView();
    //wv->setHtml(html);

    //wv->print(&printer);
    //delete wv;
    document->print(&printer);

    delete document;
}


void Stats::createInvoiceTable()
{
    int year = ui->year->currentData(Qt::UserRole).toInt();
    int month = ui->month->currentData(Qt::UserRole).toInt();

    double total_lodging_amount=0;
    double total_fooding_amount=0;

    DbMysql* d = DbMysql::getInstance();
    QMessageBox msgBox;

    if(!d->getConnection().open())
    {
        msgBox.critical(this,"Error","Failed to connect database.1");
    }
    else
    {

        QSqlQuery query1( "SELECT  inv_id,inv_number,inv_date,customer_name,room_numbers,lodging_bill_total,fooding_bill_total,(lodging_bill_total+fooding_bill_total) as amount,ps_type from invoice_details inner join payment_status on payment_status.ps_id = invoice_details.payment_status where status=0 and MONTH(inv_date) = "+QString::number(month)+" and YEAR(inv_date) = "+QString::number(year)+" order by inv_id desc",d->getConnection());
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
                    newItem->setText(query1.value(column).toString());
                    ui->invoiceTable->setItem(row, column, newItem);
                }

                total_lodging_amount+=query1.value("lodging_bill_total").toDouble();
                total_fooding_amount+=query1.value("fooding_bill_total").toDouble();

                row++;
            }
            ui->fAmount->setText(QString::number(total_fooding_amount,'f',2));
            ui->lAmount->setText(QString::number(total_lodging_amount,'f',2));
            ui->total->setText(QString::number(total_fooding_amount+total_lodging_amount,'f',2));
            ui->invoiceTable->resizeColumnsToContents();
            ui->invoiceTable->horizontalHeader()->setHighlightSections(false);
            //ui->invoiceTable->horizontalHeader()->setStyleSheet("QHeaderView::section { background-color:#0B2161;color:#fff; }");

        }
    }
}

void Stats::on_printInvoiceList_clicked()
{
    QMessageBox msgBox;
    DbMysql* d = DbMysql::getInstance();
    int year = ui->year->currentData(Qt::UserRole).toInt();
    int month = ui->month->currentData(Qt::UserRole).toInt();

    QTextDocument *document = new QTextDocument();
    QString html = "<!DOCTYPE html><html><head><title>Invoice</title><meta http-equiv=\"Content-Type\" content =\"text/html;charset=utf-8\" ><style type=\"text/css\" >table tr td { font-size:12px; } table { border-collapse: collapse; }</style></head>";
    html+= "<body style=\"color:#000;width:1024px;\" >";
    html+="<h1 align='center' >Hotel Purulia Inn</h1>";
    html+= "<h4 align='center'>Invoice List</h4><br />";
    QDate dt(year,month,1);

    html+= "<p>For "+dt.toString("MMMM yyyy")+"</p>";
    html+= "<hr />";
    html+= "<table width='100%' border=1 cellpadding='3' >";
    html+= "<tr style='background:#efefef;border:1px;border-color:#000;' ><td>#</td><td>Invoice Number</td><td>Date</td><td>Customer</td><td>Rooms</td><td>Lodge Bill</td><td>Food Bill</td><td>Total</td><td>Payment Status</td>";


    if(!d->getConnection().open())
    {
        msgBox.critical(this,"Error","Failed to connect database.1");
    }
    else
    {

        QSqlQuery query1( "SELECT  inv_id,inv_number,inv_date,customer_name,room_numbers,lodging_bill_total,fooding_bill_total,(lodging_bill_total+fooding_bill_total) as amount,ps_type from invoice_details inner join payment_status on payment_status.ps_id = invoice_details.payment_status where status=0 and MONTH(inv_date) = "+QString::number(month)+" and YEAR(inv_date) = "+QString::number(year)+" order by inv_id desc",d->getConnection());
        if(!query1.isActive())
        {
            qDebug()<<query1.lastQuery();
            msgBox.critical(this,"Error","Failed to connect database.");
        }
        else
        {
            while(query1.next())
            {
                html+= "<tr>";
                int columnSize = 9;
                for( int column = 0; column < columnSize; column++)
                {
                    html+="<td>"+query1.value(column).toString()+"</td>";
                }
                html+= "</tr>";
            }
        }
    }

    html+="</table>";

    html+= "</body></html>";


    document->setHtml(html);

    QPrinter printer;

    QPrintDialog *dialog = new QPrintDialog(&printer, this);
    if (dialog->exec() != QDialog::Accepted)
        return;

    printer.setPageMargins(5,10,5,10,QPrinter::Millimeter);
    //printer.setFullPage(true);
    document->setPageSize(QSizeF(printer.pageRect().size()));

    printer.setResolution (400);

    //QWebView* wv = new QWebView();
    //wv->setHtml(html);

    //wv->print(&printer);
    //delete wv;
    document->print(&printer);

    delete document;
}
