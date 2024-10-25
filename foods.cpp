#include "foods.h"
#include "ui_foods.h"
#include "addfoods.h"
#include "dbmysql.h"
#include "addfoodorder.h"
#include <QMessageBox>


Foods::Foods(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Foods)
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

    createFoodsTable();
    createBookingTable();
}

Foods::~Foods()
{
    delete ui;
}



void Foods::createFoodsTable()
{

    DbMysql* d = DbMysql::getInstance();
    QMessageBox msgBox;
    if(!d->getConnection().open()){
        msgBox.critical(this,"Error","Failed to connect database.");
    }
    else {
        QSqlQuery query( "select food_id,hsn_code,food_name,food_type,food_category,sgst,cgst,food_price from foods;" ,d->getConnection());
        if( !query.isActive() )
        {
            qDebug()<<"Failed to execute query. insert room.";
            qDebug()<<query.lastQuery();
            qDebug()<<query.lastError().text();
            return;
        }
        else
        {
            int row = 0 ;
            ui->foodListTable->setRowCount(query.size());
            while(query.next())
            {
                int columnSize = ui->foodListTable->columnCount();
                qDebug()<<"Column Size:"<<columnSize;

                for( int column = 1; column < columnSize+1; column++)
                {
                    QTableWidgetItem *newItem = new QTableWidgetItem();
                    newItem->setData(Qt::UserRole,QVariant::fromValue(query.value(0).toInt()));

                    //newItem->setData(Qt::,QVariant::fromValue(query1.value(columnSize).toInt()));
                    if(column >= 5 && column <= 7 ) {
                        newItem->setText(QString::number(query.value(column).toDouble(),'f',2));
                        newItem->setTextAlignment(Qt::AlignCenter);
                    }
                    else {
                    newItem->setText(query.value(column).toString());
                    }

                    qDebug()<<query.value(column).toString();
                    ui->foodListTable->setItem(row, column-1, newItem);
                }
                row++;
            }
            //ui->foodListTable->resizeColumnsToContents();
            ui->foodListTable->horizontalHeader()->setHighlightSections(false);
            //ui->foodListTable->horizontalHeader()->setStyleSheet("QHeaderView::section { background-color:#0B2161;color:#fff; }");

        }
    }
}

void Foods::createBookingTable()
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

        QSqlQuery query1( "SELECT booking_id,customer_name,room_numbers,booking_from,booking_to,reg_serial FROM booking_details where MONTH(booking_from)="+QString::number(month)+" and YEAR(booking_from)="+QString::number(year)+" order by booking_id desc;",d->getConnection());
        qDebug()<<query1.lastQuery();
        if(!query1.isActive())
        {
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

                for( int column = 0; column < columnSize; column++)
                {
                    QTableWidgetItem *newItem = new QTableWidgetItem();
                    newItem->setData(Qt::UserRole,QVariant::fromValue(query1.value(0).toInt()));

                    //newItem->setData(Qt::,QVariant::fromValue(query1.value(columnSize).toInt()));
                    newItem->setText(query1.value(column).toString());                    
                    ui->bookingTable->setItem(row, column, newItem);


                }

                row++;
            }
            //ui->checkOutTable->resizeColumnsToContents();
            ui->bookingTable->horizontalHeader()->setHighlightSections(false);
            ui->bookingTable->horizontalHeader()->setStyleSheet("QHeaderView::section { background-color:#0B2161;color:#fff; }");

        }
    }
}

void Foods::createFoodOrderTable(int bookingId)
{
    DbMysql* d = DbMysql::getInstance();
    QMessageBox msgBox;

    if(!d->getConnection().open())
    {
        msgBox.critical(this,"Error","Failed to connect database.1");
    }
    else
    {

        QSqlQuery query1( "SELECT order_id,food_name,qty,order_time,(rate*qty) FROM food_orders where booking_id="+QString::number(bookingId)+" order by order_time desc;",d->getConnection());
        qDebug()<<query1.lastQuery();
        if(!query1.isActive())
        {
            msgBox.critical(this,"Error","Failed to connect database.");
        }
        else
        {
            ui->orderTable->setRowCount(query1.size());
            int row = 0 ;
            //ui->orderTable->clear();
            qDebug()<<query1.size();
            qDebug()<<query1.lastQuery();

            while(query1.next())
            {

                int columnSize = ui->orderTable->columnCount();
                qDebug()<<"Column Size:"<<columnSize;

                for( int column = 1; column < columnSize+1; column++)
                {
                    QTableWidgetItem *newItem = new QTableWidgetItem();
                    newItem->setData(Qt::UserRole,QVariant::fromValue(query1.value(0).toInt()));

                    //newItem->setData(Qt::,QVariant::fromValue(query1.value(columnSize).toInt()));
                    newItem->setText(query1.value(column).toString());
                    ui->orderTable->setItem(row, column-1, newItem);

                }

                row++;
            }
            ui->orderTable->resizeColumnsToContents();
            ui->orderTable->horizontalHeader()->setHighlightSections(false);
            //ui->orderTable->horizontalHeader()->setStyleSheet("QHeaderView::section { background-color:#0B2161;color:#fff; }");

        }
    }
}

void Foods::on_bookingTable_cellDoubleClicked(int row, int column)
{
    int bookingId = ui->bookingTable->item(row,column)->data(Qt::UserRole).toInt();
    this->createFoodOrderTable(bookingId);

}

void Foods::on_addOrder_clicked()
{
    QMessageBox msgBox;
    if(ui->bookingTable->selectedItems().size() >0){
        int bookingId = ui->bookingTable->selectedItems().at(0)->data(Qt::UserRole).toInt();
        AddFoodOrder* fo = new AddFoodOrder();
        fo->setBookingId(bookingId);
        fo->addFoodParent(this);
        fo->getBookingDetails();
        fo->show();
    }
    else msgBox.information(this,"Add Food Order","Please select the booking");
}

void Foods::on_addFood_clicked()
{
    AddFoods* af = new AddFoods();
    af->addFoodParent(this);
    af->show();
}

void Foods::on_deleteFoods_clicked()
{
    if(ui->foodListTable->selectedItems().size() > 0)
    {
        int foodId = ui->foodListTable->selectedItems().at(0)->data(Qt::UserRole).toInt();
        DbMysql* d = DbMysql::getInstance();
        QMessageBox msgBox;

        if(!d->getConnection().open())
        {
            msgBox.critical(this,"Error","Failed to connect database.1");
        }
        else
        {

            QSqlQuery query1( "delete FROM foods where food_id="+QString::number(foodId)+";",d->getConnection());
            qDebug()<<query1.lastQuery();
            if(!query1.isActive())
            {
                msgBox.critical(this,"Error","Failed to connect database.");
            }
            else
            {
                msgBox.information(this,"Success","Food item deleted.");
                this->createFoodsTable();
            }

        }
    }
}

void Foods::on_pushButton_clicked()
{
    this->createBookingTable();
}

void Foods::on_searchBtn_clicked()
{
    QMessageBox msgBox;


    if(ui->searchBox->text().length()<=0)
    {
        msgBox.warning(this,"Search Foods","Please enter a vaild food name");
        return;
    }
    DbMysql* d = DbMysql::getInstance();
    if(!d->getConnection().open()){
        msgBox.critical(this,"Error","Failed to connect database.");
    }
    else {
        QSqlQuery query( "select * from foods where food_name like '%"+ui->searchBox->text()+"%';" ,d->getConnection());
        if( !query.isActive() )
        {
            qDebug()<<"Failed to execute query. insert room.";
            qDebug()<<query.lastQuery();
            qDebug()<<query.lastError().text();
            return;
        }
        else
        {
            int row = 0 ;
            ui->foodListTable->setRowCount(query.size());
            while(query.next())
            {
                int columnSize = ui->foodListTable->columnCount();
                qDebug()<<"Column Size:"<<columnSize;

                for( int column = 1; column < columnSize+1; column++)
                {
                    QTableWidgetItem *newItem = new QTableWidgetItem();
                    newItem->setData(Qt::UserRole,QVariant::fromValue(query.value(0).toInt()));

                    //newItem->setData(Qt::,QVariant::fromValue(query1.value(columnSize).toInt()));
                    newItem->setText(query.value(column).toString());
                    qDebug()<<query.value(column).toString();
                    ui->foodListTable->setItem(row, column-1, newItem);
                }
                row++;
            }
            //ui->foodListTable->resizeColumnsToContents();
            ui->foodListTable->horizontalHeader()->setHighlightSections(false);
            //ui->foodListTable->horizontalHeader()->setStyleSheet("QHeaderView::section { background-color:#0B2161;color:#fff; }");

        }
    }
}

void Foods::on_reload_clicked()
{
    this->createFoodsTable();
}

void Foods::on_removeOrder_clicked()
{
    QMessageBox msgBox;
    if(ui->orderTable->selectedItems().size() > 0)
    {
        DbMysql* d = DbMysql::getInstance();
        if(!d->getConnection().open()){
            msgBox.critical(this,"Error","Failed to connect database.");
        }
        else
        {
            QSqlQuery query( "delete from food_orders where order_id="+ui->orderTable->selectedItems().at(0)->data(Qt::UserRole).toString()+";" ,d->getConnection());
            if( !query.isActive() )
            {
                qDebug()<<"Failed to execute query. insert room.";
                qDebug()<<query.lastQuery();
                qDebug()<<query.lastError().text();
                return;
            }
            else
            {
                qDebug()<<query.lastQuery();
                msgBox.information(this,"Delete Order","Order deleted.");
                int bookingId = ui->bookingTable->selectedItems().at(0)->data(Qt::UserRole).toInt();
                this->createFoodOrderTable(bookingId);

            }
        }
    }
    else msgBox.information(this,"Delete Order","Please select a order.");

}

void Foods::on_finalizeFoods_clicked()
{
    QMessageBox msgBox;
    if(ui->bookingTable->selectedItems().size() > 0)
    {
        DbMysql* d = DbMysql::getInstance();
        if(!d->getConnection().open()){
            msgBox.critical(this,"Error","Failed to connect database.");
        }
        else
        {
            QSqlQuery query( "select * from invoice where booking_id="+ui->bookingTable->selectedItems().at(0)->data(Qt::UserRole).toString()+" and status = 0;" ,d->getConnection());
            if( !query.isActive() )
            {
                qDebug()<<"Failed to execute query. insert room.";
                qDebug()<<query.lastQuery();
                qDebug()<<query.lastError().text();
                return;
            }
            else
            {
                qDebug()<<query.lastQuery();
                if(query.size() > 0)
                {
                     QSqlQuery query2( "select sum(qty*rate) as food_bill from food_orders where booking_id="+ui->bookingTable->selectedItems().at(0)->data(Qt::UserRole).toString()+";" ,d->getConnection());
                     if( !query2.isActive() )
                     {
                         qDebug()<<"Failed to execute query. insert room.";
                         qDebug()<<query2.lastQuery();
                         qDebug()<<query2.lastError().text();
                         return;
                     }
                     else
                     {
                         query2.next();
                         qDebug()<<query2.value("food_bill").toString();
                         QSqlQuery query3( "update invoice set fooding_bill_total="+query2.value("food_bill").toString()+" where booking_id="+ui->bookingTable->selectedItems().at(0)->data(Qt::UserRole).toString()+";" ,d->getConnection());
                         if( !query2.isActive() )
                         {
                             qDebug()<<"Failed to execute query. insert room.";
                             qDebug()<<query2.lastQuery();
                             qDebug()<<query2.lastError().text();
                             return;
                         }
                         else
                         {
                             msgBox.information(this,"Update Invoice","Invoice updated.");
                         }

                     }
                }
            }
        }
    }
    else msgBox.information(this,"Update Invoice","Please select a booking.");
}

void Foods::on_editFoodDetails_clicked()
{
    QMessageBox msgBox;
    if(ui->foodListTable->selectedItems().size() <= 0 )
    {
         msgBox.information(this,"Edit Food & Beverages Details","Please select a food from the list.");
         return;
    }

    AddFoods *foodObj = new AddFoods();
    //foodObj->setParent(this);
    qDebug()<<"ID:"<<ui->foodListTable->selectedItems().at(0)->data(Qt::UserRole).toInt();
    foodObj->addFoodParent(this);
    foodObj->setFoodId(ui->foodListTable->selectedItems().at(0)->data(Qt::UserRole).toInt());
    foodObj->setEditMode(true);
    foodObj->show();

}
