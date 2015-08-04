#include "bookingcalendar.h"
#include "ui_bookingcalendar.h"
#include "dbmysql.h"
#include "createbooking.h"
#include "createinvoices.h"
#include "createlog.h"
#include "addadvancepayment.h"
#include <QtPrintSupport/QPrinter>
#include <QTextDocument>
#include <QtPrintSupport/QPrintDialog>
#include <QMessageBox>
#include <QPainter>
#include <QPrintDialog>
#include <QTextCursor>
#include <QMargins>
#include <QTextBlockFormat>
#include <QDebug>
#include <QScrollBar>
#include <QMessageBox>
#include <QHash>

BookingCalendar::BookingCalendar(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::BookingCalendar)
{
    ui->setupUi(this);
    QDate cDate = QDate::currentDate();
    QDate startDate = cDate.addMonths(-5);
    QDate endDate = cDate.addMonths(5);

    ui->calenderFilterFrom->setDate(startDate);
    ui->calenderFilterTo->setDate(endDate);
    ui->bookingLogFilterFrom->setDate(startDate);
    ui->bookingLogFilterTo->setDate(endDate);
    this->populateBookingLog();
}

BookingCalendar::~BookingCalendar()
{
    delete ui;
}


void BookingCalendar::createCalendar()
{

    DbMysql* d = DbMysql::getInstance();
    QMessageBox msgBox;
    if(!d->getConnection().open()){
        msgBox.critical(this,"Error","Failed to connect database.1");
    }
    else{
        QSqlQuery query1( "select room_id,room_number from rooms order by room_id;" ,d->getConnection());
        if(!query1.isActive()){
            msgBox.critical(this,"Error","Failed to connect database.");
        }
        else {
            int row_num = query1.size();
            ui->bookingCalendarTable->setRowCount(row_num);
            QStringList roomNums;
            QHash<QString,int> row_index;
            QHash<int,QString> room_index;
            int i = 0;
            while(query1.next()){
                roomNums.append(query1.value(1).toString());
                row_index.insert(query1.value(1).toString(),i);
                qDebug()<<"room:"<<query1.value(1).toString()<<" index:"<<i;
                room_index.insert(query1.value(0).toInt(),query1.value(1).toString());
                i++;
            }
            ui->bookingCalendarTable->setVerticalHeaderLabels(roomNums);
            QDate date,cur_date;
            date = QDate::currentDate();
            cur_date = QDate::currentDate();

            QString start_date = date.addMonths(-1).toString("yyyy-MM-dd"); //QDate::fromString(date.addMonths(-1).toString(),"Y-m-d H:i:s").toString();
            QString end_date = date.addMonths(5).toString("yyyy-MM-dd");//QDate::fromString(date.addMonths(5).toString(),"Y-m-d H:i:s").toString();

            int total_days = date.addMonths(-1).daysTo(date.addMonths(5));
            date = date.addMonths(-1);
            int count = total_days;
            int index=0;
            int scroll_pos = 0;
            QStringList hLabels;

            QHash<QString,int> coloumn_index;

            while(count > 0){
                date = date.addDays(1);
                hLabels.append(date.toString());
                coloumn_index.insert(date.toString(),index);
                if(date == cur_date){
                    scroll_pos = index;
                }                
                index++;
                count--;
            }

            for(int l = 0;l<i;l++){
                for(int j=0;j<total_days;j++){
                    QTableWidgetItem *newItem = new QTableWidgetItem();
                    newItem->setText("");

                    ui->bookingCalendarTable->setItem(l,j, newItem);

                }
            }

            ui->bookingCalendarTable->setColumnCount(total_days);
            ui->bookingCalendarTable->setHorizontalHeaderLabels(hLabels);
            /* Sets horizontal font size */
            QFont font = ui->bookingCalendarTable->horizontalHeader()->font();
            font.setPointSize(9);
            ui->bookingCalendarTable->horizontalHeader()->setFont( font );

            /*Sets vertical header background*/

            //ui->bookingCalendarTable->verticalHeader()->setStyleSheet("QHeaderView::section { background-color:#0B2161;color:#fff; }");
            //ui->bookingCalendarTable->horizontalHeader()->setStyleSheet("QHeaderView::section { background-color:#0B2161;color:#fff; }");

            int mnth = QDate::currentDate().month();
            ui->bookingCalendarTable->horizontalScrollBar()->setValue(scroll_pos);

            QSqlQuery query2("select * from room_bookings where booking_from >='"+start_date+"' and booking_to<='"+end_date+"' order by room_id;");
            if(!query2.isActive()){

                qDebug()<<query2.lastError();
                msgBox.critical(this,"Error","Failed to connect database.2");
            }
            else {
                qDebug()<<query2.lastQuery();
                while(query2.next()){
                    if(row_index.contains(room_index.value(query2.value("room_id").toInt()))){
                        int row_i = row_index.value(room_index.value(query2.value("room_id").toInt()));
                        //qDebug()<<"room :"<<room_index.value(query2.value(2).toInt())<<" index:"<<row_i;
                        if(coloumn_index.contains(query2.value(3).toDate().toString()) && coloumn_index.contains(query2.value(4).toDate().toString()))
                        {
                            int clmn_start = coloumn_index.value(query2.value(3).toDate().toString());
                            int clmn_end =  coloumn_index.value(query2.value(4).toDate().toString());

                            for(int k = clmn_start ; k<=clmn_end; k++){
                                QTableWidgetItem *newItem = new QTableWidgetItem();
                                newItem->setText("Booked");
                                newItem->setTextAlignment(Qt::AlignCenter);
                                //qDebug()<<"Data found: row:"<<row_i<<" column:"<<clmn_start<<","<<clmn_end;
                                QColor clr;
                                clr.setNamedColor("green");
                                newItem->setBackgroundColor(clr);
                                ui->bookingCalendarTable->setItem(row_i,k, newItem);

                            }

                        }

                    }
                }
            }

            //QTableWidgetItem *newItem = new QTableWidgetItem();

            //ui->bookingCalendarTable->scrollToItem(ui->bookingCalendarTable->item(1,50),QAbstractItemView::EnsureVisible );
        }

    }
}

void BookingCalendar::createCalendar(QDate aFromDate,QDate aToDate)
{

    DbMysql* d = DbMysql::getInstance();
    QMessageBox msgBox;
    if(!d->getConnection().open()){
        msgBox.critical(this,"Error","Failed to connect database.1");
    }
    else{
        QSqlQuery query1( "select room_id,room_number from rooms order by room_id;" ,d->getConnection());
        if(!query1.isActive()){
            msgBox.critical(this,"Error","Failed to connect database.");
        }
        else {
            int row_num = query1.size();
            ui->bookingCalendarTable->setRowCount(row_num);
            QStringList roomNums;
            QHash<QString,int> row_index;
            QHash<int,QString> room_index;
            int i = 0;
            while(query1.next()){
                roomNums.append(query1.value(1).toString());
                row_index.insert(query1.value(1).toString(),i);
                qDebug()<<"room:"<<query1.value(1).toString()<<" index:"<<i;
                room_index.insert(query1.value(0).toInt(),query1.value(1).toString());
                i++;
            }
            ui->bookingCalendarTable->setVerticalHeaderLabels(roomNums);
            QDate date,cur_date;
            date = QDate::currentDate();
            cur_date = QDate::currentDate();

            QString start_date = aFromDate.toString("yyyy-MM-dd"); //QDate::fromString(date.addMonths(-1).toString(),"Y-m-d H:i:s").toString();
            QString end_date = aToDate.toString("yyyy-MM-dd");//QDate::fromString(date.addMonths(5).toString(),"Y-m-d H:i:s").toString();

            int total_days = aFromDate.daysTo(aToDate);
            date = aFromDate;
            int count = total_days;
            int index=0;
            int scroll_pos = 0;
            QStringList hLabels;

            QHash<QString,int> coloumn_index;

            while(count > 0){
                date = date.addDays(1);
                hLabels.append(date.toString());
                coloumn_index.insert(date.toString(),index);
                if(date == cur_date){
                    scroll_pos = index;
                }
                index++;
                count--;
            }

            for(int l = 0;l<i;l++){
                for(int j=0;j<total_days;j++){
                    QTableWidgetItem *newItem = new QTableWidgetItem();
                    newItem->setText("");

                    ui->bookingCalendarTable->setItem(l,j, newItem);

                }
            }

            ui->bookingCalendarTable->setColumnCount(total_days);
            ui->bookingCalendarTable->setHorizontalHeaderLabels(hLabels);
            /* Sets horizontal font size */
            QFont font = ui->bookingCalendarTable->horizontalHeader()->font();
            font.setPointSize(9);
            ui->bookingCalendarTable->horizontalHeader()->setFont( font );

            /*Sets vertical header background*/

            //ui->bookingCalendarTable->verticalHeader()->setStyleSheet("QHeaderView::section { background-color:#0B2161;color:#fff; }");
            //ui->bookingCalendarTable->horizontalHeader()->setStyleSheet("QHeaderView::section { background-color:#0B2161;color:#fff; }");


            ui->bookingCalendarTable->horizontalScrollBar()->setValue(scroll_pos);
            ui->bookingCalendarTable->horizontalHeader()->setHighlightSections(false);
            QSqlQuery query2("select * from room_bookings where booking_from >='"+start_date+"' and booking_to<='"+end_date+"' order by room_id;");
            if(!query2.isActive()){

                qDebug()<<query2.lastError();
                msgBox.critical(this,"Error","Failed to connect database.2");
            }
            else {
                qDebug()<<query2.lastQuery();
                while(query2.next()){
                    if(row_index.contains(room_index.value(query2.value("room_id").toInt()))){
                        int row_i = row_index.value(room_index.value(query2.value("room_id").toInt()));
                        //qDebug()<<"room :"<<room_index.value(query2.value(2).toInt())<<" index:"<<row_i;
                        if(coloumn_index.contains(query2.value(3).toDate().toString()) && coloumn_index.contains(query2.value(4).toDate().toString()))
                        {
                            int clmn_start = coloumn_index.value(query2.value(3).toDate().toString());
                            int clmn_end =  coloumn_index.value(query2.value(4).toDate().toString());

                            for(int k = clmn_start ; k<=clmn_end; k++){
                                QTableWidgetItem *newItem = new QTableWidgetItem();
                                newItem->setText("Booked");
                                newItem->setTextAlignment(Qt::AlignCenter);
                                //qDebug()<<"Data found: row:"<<row_i<<" column:"<<clmn_start<<","<<clmn_end;
                                QColor clr;
                                clr.setNamedColor("green");
                                newItem->setBackgroundColor(clr);
                                ui->bookingCalendarTable->setItem(row_i,k, newItem);

                            }

                        }

                    }
                }
            }

            //QTableWidgetItem *newItem = new QTableWidgetItem();

            //ui->bookingCalendarTable->scrollToItem(ui->bookingCalendarTable->item(1,50),QAbstractItemView::EnsureVisible );
        }

    }
}

void BookingCalendar::createBookingLogTable()
{

}

void BookingCalendar::on_createBooking_clicked()
{
    QList<QTableWidgetSelectionRange> wir = ui->bookingCalendarTable->selectedRanges();

    QItemSelectionModel  *select = ui->bookingCalendarTable->selectionModel();



    foreach(QModelIndex  rs,select->selectedIndexes()){
        qDebug()<<QString::number(rs.row())+" "+QString::number(rs.column());
        QString coloumn_name = ui->bookingCalendarTable->horizontalHeaderItem(rs.column())->text();
        QString row_name = ui->bookingCalendarTable->verticalHeaderItem(rs.row())->text();

        qDebug()<<coloumn_name+","+row_name;
    }

    CreateBooking* cb = new CreateBooking();
    cb->setParent(this);
    cb->show();
}

void BookingCalendar::populateBookingLog()
{
    DbMysql* d = DbMysql::getInstance();
    QMessageBox msgBox;

    if(!d->getConnection().open())
    {
        msgBox.critical(this,"Error","Failed to connect database.1");
    }
    else
    {
        QString from_range_date = ui->bookingLogFilterFrom->date().toString("yyyy-MM-dd");
        QString to_range_date = ui->bookingLogFilterTo->date().toString("yyyy-MM-dd");
        QSqlQuery query1( "select booking_id,booking_date,customer_name,room_numbers,booking_from,booking_to,reg_serial,in_time,out_time,nop from booking_details where booking_from >='"+from_range_date+"' and  booking_from<='"+to_range_date+"' order by booking_id desc;" ,d->getConnection());
        if(!query1.isActive())
        {
            qDebug()<<query1.lastQuery();
            msgBox.critical(this,"Error","Failed to connect database.");
        }
        else
        {
            qDebug()<<query1.lastQuery();
            ui->bookingLogTable->setRowCount(query1.size());
            int row = 0 ;

            while(query1.next())
            {
                int columnSize = ui->bookingLogTable->columnCount();
                qDebug()<<"Column Size:"<<columnSize;

                for( int column = 0; column < columnSize-1; column++)
                {
                    QTableWidgetItem *newItem = new QTableWidgetItem();
                    newItem->setData(Qt::UserRole,QVariant::fromValue(query1.value(0).toInt()));
                    if(column == 5)
                    {
                        if(query1.value(4).toDate() != query1.value(5).toDate())
                        {
                            newItem->setText(query1.value(column).toString());
                        }
                    }
                    else newItem->setText(query1.value(column).toString());
                    ui->bookingLogTable->setItem(row, column, newItem);

                }

                QDate frm_Date = QDate::fromString(query1.value(4).toString(),"yyyy-MM-dd");

                QDate to_Date = QDate::fromString(query1.value(5).toString(),"yyyy-MM-dd");



                int numDays = frm_Date.daysTo(to_Date)+1;

                qDebug()<<numDays<<" "<<frm_Date<<" "<<to_Date;

                QTableWidgetItem *newItem = new QTableWidgetItem();
                newItem->setData(Qt::UserRole,QVariant::fromValue(query1.value(0).toInt()));
                newItem->setText(QString::number(numDays));
                ui->bookingLogTable->setItem(row,columnSize-1, newItem);

                row++;
            }
            ui->bookingLogTable->resizeColumnsToContents();
            ui->bookingLogTable->horizontalHeader()->setHighlightSections(false);
            //ui->bookingLogTable->horizontalHeader()->setStyleSheet("QHeaderView::section { background-color:#0B2161;color:#fff; }");
        }
    }
}






void BookingCalendar::on_bookingLogTable_cellDoubleClicked(int row, int column)
{
    QMessageBox msgBox;
    int booking_id = ui->bookingLogTable->item(row,0)->data(Qt::UserRole).toInt();
    qDebug()<<ui->bookingLogTable->item(row,6)->text().trimmed().length()<<"  "<<ui->bookingLogTable->item(row,6)->text();
    if(ui->bookingLogTable->item(row,6)->text().trimmed().length() <= 0)
    {
        CreateLog* cl = new CreateLog();
        cl->setBookingCalObject(this);
        cl->setAttribute(Qt::WA_DeleteOnClose);
        cl->setDataFields(booking_id);
        cl->show();
    }
    else
    {
         msgBox.information(this,"Information","Customer already checked in.");
    }
}


void BookingCalendar::on_deleteBooking_clicked()
{
    QMessageBox msgBox;
    if(ui->bookingLogTable->selectedItems().size())
    {
        int row = ui->bookingLogTable->selectedItems().at(0)->row();

        int booking_id = ui->bookingLogTable->item(row,0)->data(Qt::UserRole).toInt();

        QMessageBox::StandardButton reply;

        reply = QMessageBox::question(this, "Delete Booking", "Are you want to delete this booking?",QMessageBox::Yes|QMessageBox::No);
        if (reply == QMessageBox::Yes) {

            DbMysql* d = DbMysql::getInstance();
            if (!d->getConnection().open()) {
                msgBox.critical(this,"Error","Failed to connect database.");
            }
            else {
                QSqlQuery query("delete from booking where booking_id="+QString::number(booking_id)+";",d->getConnection());
                if( !query.isActive() )
                {
                    msgBox.critical(this,"Error","Failed to execute query.");
                    qDebug()<<query.lastError().text();
                    return;
                }
                else{
                    QSqlQuery query1("delete from room_bookings where booking_id="+QString::number(booking_id)+";",d->getConnection());
                    QSqlQuery query5("delete from food_orders where booking_id="+QString::number(booking_id)+";",d->getConnection());
                    QSqlQuery query3("delete from advance_payments where booking_id="+QString::number(booking_id)+";",d->getConnection());
                    if(query1.isActive() && query5.isActive())
                    {
                        if(query3.isActive())
                        {

                        }
                        msgBox.information(this,"Information","Booking Deleted.");
                        ui->bookingCalendarTable->clear();

                        this->createCalendar();
                        this->populateBookingLog();
                    }
                    //this->close();
                    //this->show();
                }
            }
        }
    }
    else
        msgBox.information(this,"Delete Booking","Please select a booking from booking table.");
}



void BookingCalendar::on_calendarFilter_clicked()
{
    this->createCalendar(ui->calenderFilterFrom->date(),ui->calenderFilterTo->date());
}

void BookingCalendar::on_bookingLogFilter_clicked()
{
    this->populateBookingLog();
}

void BookingCalendar::on_printBookingConfirm_clicked()
{
    QMessageBox msgBox;
    if(ui->bookingLogTable->selectedItems().size() > 0)
    {
        int booking_id = ui->bookingLogTable->selectedItems().at(0)->data(Qt::UserRole).toInt();

        QTextDocument *document = new QTextDocument();
        QString html = "<!DOCTYPE html><html><head><title>Invoice</title><meta http-equiv=\"Content-Type\" content =\"text/html;charset=utf-8\" ><style type=\"text/css\" >table tr td { font-size:12px; } \n table { border-collapse: collapse; }</style></head>";
        html+= "<body style=\"color:#000;\" >";
        DbMysql* d = DbMysql::getInstance();
        if(!d->getConnection().open())
        {
            msgBox.critical(this,"Error","Failed to connect database.1");
        }
        else
        {

            QSqlQuery query1( "SELECT  * from hotel_info;",d->getConnection());
            qDebug()<<query1.lastQuery();
            if(!query1.isActive())
            {
                msgBox.critical(this,"Error","Failed to connect database.");
            }
            else
            {
                query1.next();
                html+= "<h1 align=\"center\" style='font-size:30px' >"+query1.value("hotel_name").toString()+"</h1>";
                html+= "<p align=\"center\" >"+query1.value("address").toString()+"<br />"+query1.value("dist").toString()+","+query1.value("state").toString()+","+query1.value("pin").toString()+"</p>";
                html+= "<p align=\"center\" >Phone: "+query1.value("phone1").toString()+"/"+query1.value("phone2").toString()+" Email: "+query1.value("email").toString()+" Website: "+query1.value("website").toString()+"</p>";

                html+="<br />";
                html+= "<hr style='border-bottom:5px solid #666' />";
            }
        }
        html+= "<p align='right' >Date:"+QDateTime::currentDateTime().toString("dd-MM-yyyy HH:mm:ss")+"</p>";
        html+= "<h2 align=\"center\" >Booking Confirmation</h2><br />";

        html+= "<table cellpadding=\"5\" border=1 cellspacing=1  width=\"100%\" >";
        if(!d->getConnection().open())
        {
            msgBox.critical(this,"Error","Failed to connect database.1");
        }
        else
        {

            QSqlQuery query2( "SELECT  * from booking_details where booking_id ="+QString::number(booking_id)+";",d->getConnection());
            qDebug()<<query2.lastQuery();
            if(!query2.isActive())
            {
                msgBox.critical(this,"Error","Failed to connect database.");
            }
            else
            {
                query2.next();
                html+= "<tr><td colspan=\"2\" align=\"left\" ><b>Customer Name :</b> "+query2.value("customer_name").toString()+" </td></tr>";
                html+= "<tr><td colspan=\"2\" align=\"left\" ><b>Address : </b>"+query2.value("address").toString()+" </td></tr>";
                html+= "<tr><td align=\"left\" ><b>Phone : </b>"+query2.value("phone").toString()+" </td><td><b>Email: </b>"+query2.value("email").toString()+"</td></tr>";
                html+= "<tr><td align=\"left\" ><b>ID Type : </b>"+query2.value("id_type").toString()+" </td><td><b>ID Serial : </b>"+query2.value("id_serial").toString()+"</td></tr>";
            }
        }
        html+= "</table><br /><br />";

        html+= "<table cellpadding=\"5\" border=1 cellspacing=1  width=\"100%\" >";
        html+= "<tr style='font-weight:bold;background:#efefef;' ><td>Room Number</td><td>Room Type</td><td>Booking From</td><td>Booking To</td><td>Rate</td></tr>";
        double room_total_amount = 0;
        if(!d->getConnection().open())
        {
            msgBox.critical(this,"Error","Failed to connect database.1");
        }
        else
        {

            QSqlQuery query3( " select * from room_booking_details where booking_id="+QString::number(booking_id)+";",d->getConnection());
            qDebug()<<query3.lastQuery();
            if(!query3.isActive())
            {
                msgBox.critical(this,"Error","Failed to connect database.");
            }
            else
            {
                while(query3.next())
                {
                    html+= "<tr><td>"+query3.value("room_number").toString()+" </td>";
                    html+= "<td>"+query3.value("room_type").toString()+" </td>";
                    html+= "<td>"+query3.value("booking_from").toDate().toString("dd-MM-yyyy")+" </td>";
                    QString to = (query3.value("booking_to").toString() == query3.value("booking_from").toString())?" ":query3.value("booking_to").toDate().toString("dd-MM-yyyy");
                    html+= "<td>"+to+" </td>";

                    html+= "<td>"+query3.value("room_rate").toString()+" </td>";
                    html+= "</tr>";

                }
            }
        }
        html+= "</table>";

        html+= "<br /><hr />";
        html+= "<table><tr><td width='25%' ></td><td width='25%' ></td><td width='25%' ></td><td width='25%' ><p><i>Signature</i></p></td></tr></table>";
        html+= "<br /><br /><br /><br /><hr />";
        html+= "<h4 align='center' ><u>Cancellation/amendment policy</u></h4>";
        html+= "<p><b><i>Individual reservations:</i></p>";
        html+= "<p>For Cancellations/ amendments made after 12 noon, 3 days prior to check-in date, One night’s room rate is charged.</p>";
        html+= "<p>For No shows and early check outs, One night’s room rate is charged.</p>";
        html+= "<p><b><i>Groups / Conference bookings:</i></p>";
        html+= "<p>In case there is any no-show or cancellation/amendment of the conference/group (in part or full), within 15 days or less from the date of check in, a retention charge will be levied.</p>";
        html+= "<p>The retention charge will be calculated as follows - Number of rooms being canceled/no-shows X 1 night X applicable daily rate per room for the conference/group.</p>";
        html+= "<p>In case of Early check out retention will be charged for those nights booked, now being released due to the early check out.</p>";


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
    else
    {
        msgBox.information(this,"Print Booking Confirmation","Please select a booking from booking table.");
    }
}

void BookingCalendar::on_checkOutBtn_clicked()
{
    QMessageBox msgBox;
    if(ui->bookingLogTable->selectedItems().size() > 0)
    {
        int booking_id  = ui->bookingLogTable->selectedItems().at(0)->data(Qt::UserRole).toInt();
        DbMysql* d = DbMysql::getInstance();
        if(!d->getConnection().open())
        {
            msgBox.critical(this,"Error","Failed to connect database.1");
        }
        else
        {

            QSqlQuery query1( "update booking set out_time='"+QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss")+"' where booking_id="+QString::number(booking_id)+";",d->getConnection());
            qDebug()<<query1.lastQuery();
            if(!query1.isActive())
            {
                msgBox.critical(this,"Error","Failed to connect database.");
            }
            else
            {
                QSqlQuery query("SELECT GROUP_CONCAT(room_id) from room_bookings WHERE booking_id ="+QString::number(booking_id)+" group by booking_id");
                if(query1.isActive())
                {
                    query.next();
                    QSqlQuery query2("update rooms set room_state=0 where room_id in ("+query.value(0).toString()+"); ",d->getConnection());
                    if(query2.isActive())
                    {
                        msgBox.information(this,"Booking Check Out","Customer Check Out Done.");
                    }
                    else qDebug()<<query2.lastQuery();
                }
                else qDebug()<<query.lastQuery();
            }
        }
    }
    else
    {
        msgBox.information(this,"Booking Check Out","Please select a booking from booking table.");
    }
}

void BookingCalendar::on_createInvoice_clicked()
{
    QMessageBox msgBox;
    if( ui->bookingLogTable->selectedItems().size() > 0)
    {
        int booking_id = ui->bookingLogTable->selectedItems().at(0)->data(Qt::UserRole).toInt();
        CreateInvoices* cInv =  new CreateInvoices();
        cInv->addBookingData(booking_id);
        cInv->show();
    }
    else
    {
        msgBox.warning(this,"Create Invoice","Please select a booking to create invoice.");
        return;
    }
}

void BookingCalendar::on_searchButton_clicked()
{
    DbMysql* d = DbMysql::getInstance();
    QMessageBox msgBox;

    if(!d->getConnection().open())
    {
        msgBox.critical(this,"Error","Failed to connect database.1");
    }
    else
    {
        QString from_range_date = ui->bookingLogFilterFrom->date().toString("yyyy-MM-dd");
        QString to_range_date = ui->bookingLogFilterTo->date().toString("yyyy-MM-dd");
        QSqlQuery query1( "select booking_id,booking_date,customer_name,room_numbers,booking_from,booking_to,reg_serial,in_time,out_time,nop from booking_details where booking_from >='"+from_range_date+"' and  booking_from<='"+to_range_date+"' and customer_name like '%"+ui->customerName->text()+"%' order by booking_id desc;" ,d->getConnection());
        if(!query1.isActive())
        {
            qDebug()<<query1.lastQuery();
            msgBox.critical(this,"Error","Failed to connect database.");
        }
        else
        {
            qDebug()<<query1.lastQuery();
            ui->bookingLogTable->setRowCount(query1.size());
            int row = 0 ;

            while(query1.next())
            {
                int columnSize = ui->bookingLogTable->columnCount();
                qDebug()<<"Column Size:"<<columnSize;

                for( int column = 0; column < columnSize-1; column++)
                {
                    QTableWidgetItem *newItem = new QTableWidgetItem();
                    newItem->setData(Qt::UserRole,QVariant::fromValue(query1.value(0).toInt()));
                    newItem->setText(query1.value(column).toString());
                    ui->bookingLogTable->setItem(row, column, newItem);

                }

                QDate frm_Date = QDate::fromString(query1.value(4).toString(),"yyyy-MM-dd");
                QDate to_Date = QDate::fromString(query1.value(5).toString(),"yyyy-MM-dd");



                int numDays = frm_Date.daysTo(to_Date)+1;

                qDebug()<<numDays<<" "<<frm_Date<<" "<<to_Date;

                QTableWidgetItem *newItem = new QTableWidgetItem();
                newItem->setData(Qt::UserRole,QVariant::fromValue(query1.value(0).toInt()));
                newItem->setText(QString::number(numDays));
                ui->bookingLogTable->setItem(row,columnSize-1, newItem);

                row++;
            }
            ui->bookingLogTable->resizeColumnsToContents();
            ui->bookingLogTable->horizontalHeader()->setHighlightSections(false);
            //ui->bookingLogTable->horizontalHeader()->setStyleSheet("QHeaderView::section { background-color:#0B2161;color:#fff; }");
        }
    }
    ui->customerName->setText("");
}

void BookingCalendar::on_editBooking_clicked()
{
    QMessageBox msgBox;
    if(ui->bookingLogTable->selectedItems().size() > 0)
    {
        CreateBooking* cb = new CreateBooking();
        cb->setParent(this);
        cb->setBookingId(ui->bookingLogTable->selectedItems().at(0)->data(Qt::UserRole).toInt());
        cb->setEditMode(true);
        cb->show();
    }
    else
    {
        msgBox.warning(this,"Edit Booking","Please select a booking for edit.");
        return;
    }
}

void BookingCalendar::on_advPaymentBtn_clicked()
{
    QMessageBox msgBox;
    if(ui->bookingLogTable->selectedItems().size() > 0)
    {
        AddAdvancePayment* ap = new AddAdvancePayment();
        ap->setBookingId(ui->bookingLogTable->selectedItems().at(0)->data(Qt::UserRole).toInt());
        ap->getAllPayments();
        ap->show();
    }
    else
    {
        msgBox.warning(this,"Edit Booking","Please select a booking for edit.");
        return;
    }

}
