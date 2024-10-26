#include "addfoodorder.h"
#include "ui_addfoodorder.h"
#include "dbmysql.h"
#include <QMessageBox>
#include <QHash>
#include <QCompleter>
#include <QStandardItem>

AddFoodOrder::AddFoodOrder(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AddFoodOrder)
{
    ui->setupUi(this);
    addFoodsInForm();
}



AddFoodOrder::~AddFoodOrder()
{
    delete ui;
}


void AddFoodOrder::addFoodParent(Foods *parent)
{
    this->m_parent = parent;
}

void AddFoodOrder::setBookingId(int bookingId)
{
    this->booking_id = bookingId;
}


void AddFoodOrder::getBookingDetails()
{
    if(this->booking_id > 0)
    {
        DbMysql* d = DbMysql::getInstance();
        QMessageBox msgBox;

        if(!d->getConnection().open())
        {
            msgBox.critical(this,"Error","Failed to connect database.1");
        }
        else
        {
            qDebug()<<this->booking_id;
            QSqlQuery query1( "SELECT booking_id,customer_name,room_numbers FROM booking_details where booking_id="+QString::number(this->booking_id)+" order by booking_id desc",d->getConnection());
            qDebug()<<query1.lastQuery();
            if(!query1.isActive())
            {
                msgBox.critical(this,"Error","Failed to connect database.");
            }
            else
            {
                while(query1.next())
                {
                    ui->booking_serial->setText(query1.value(0).toString());
                    ui->customer_name->setText(query1.value(1).toString());
                    ui->room_no->setText(query1.value(2).toString());
                }
            }
        }
    }
}

void AddFoodOrder::addFoodsInForm()
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

        QSqlQuery query1( "select food_id,food_name from foods order by food_id desc;" ,d->getConnection());
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
                //ui->customer_list->addItem(query1.value(1).toString(),QVariant::fromValue(query1.value(0).toInt()));
                //ui->roomsList->addItem(query1.value(1).toString(),QVariant::fromValue(query1.value(0).toInt()));
            }

        }

    }
    cl->setCaseSensitivity(Qt::CaseInsensitive);
    cl->setModel(model);
    ui->foodItem->setCompleter(cl);
    connect(cl, SIGNAL(activated(QModelIndex)),this, SLOT(onItemActivated(QModelIndex)));
}

void AddFoodOrder::onItemActivated(QModelIndex index)
{
    int food_id = index.data(Qt::UserRole).toInt();
    DbMysql* d = DbMysql::getInstance();
    QMessageBox msgBox;
    if(!d->getConnection().open()){
        msgBox.critical(this,"Error","Failed to connect database.");
    }
    else {
        QSqlQuery query( "select food_price from foods where food_id="+QString::number(food_id)+";" ,d->getConnection());
        if( !query.isActive() )
        {
            qDebug()<<"Failed to execute query. insert room.";
            qDebug()<<query.lastQuery();
            qDebug()<<query.lastError().text();
            return;
        }
        else
        {
            //int row = ui->foodOrderList->rowCount()+1;
            //ui->foodOrderList->setRowCount(row);
            while(query.next())
            {
                ui->price->setText(QString::number(query.value(0).toInt(),'f',2));

            }
        }
    }
}

void AddFoodOrder::on_cancelBtn_clicked()
{
    this->close();
}

void AddFoodOrder::on_addFoodInOrderListBtn_clicked()
{
        qDebug()<<"price:"<<ui->price->text().toInt()<<" qty:"<<ui->qty->text().toInt();

        int row = ui->foodOrderList->rowCount();
        ui->foodOrderList->setRowCount(row+1);

        DbMysql* d = DbMysql::getInstance();
        QMessageBox msgBox;
        if(!d->getConnection().open()){
            msgBox.critical(this,"Error","Failed to connect database.");
        }
        else {

            QSqlQuery query( "select * from foods where food_id="+QString::number(ui->foodItem->completer()->currentIndex().data(Qt::UserRole).toInt())+";" ,d->getConnection());
            if( !query.isActive() )
            {
                qDebug()<<"Failed to execute query. Add Food Order.";
                qDebug()<<query.lastQuery();
                qDebug()<<query.lastError().text();
                msgBox.critical(this,"Error",query.lastError().text());
                return;
            }
            else
            {
                query.next();


                QTableWidgetItem *item = new QTableWidgetItem();
                item->setText(query.value("hsn_code").toString());
                item->setData(Qt::UserRole,QVariant::fromValue(ui->foodItem->completer()->currentIndex().data(Qt::UserRole).toInt()));
                ui->foodOrderList->setItem(row,0,item);

                QTableWidgetItem *item1 = new QTableWidgetItem();
                item1->setText(query.value("food_name").toString());
                item1->setData(Qt::UserRole,QVariant::fromValue(ui->foodItem->completer()->currentIndex().data(Qt::UserRole).toInt()));
                ui->foodOrderList->setItem(row,1,item1);

                QTableWidgetItem *item2 = new QTableWidgetItem();
                item2->setText(ui->qty->text());
                item2->setData(Qt::UserRole,QVariant::fromValue(ui->qty->text().toInt()));
                ui->foodOrderList->setItem(row,2,item2);

                QTableWidgetItem *item3 = new QTableWidgetItem();
                item3->setText(query.value("food_price").toString());
                ui->foodOrderList->setItem(row,3,item3);

                QTableWidgetItem *item4 = new QTableWidgetItem();
                item4->setText(query.value("sgst").toString());
                ui->foodOrderList->setItem(row,4,item4);

                QTableWidgetItem *item5 = new QTableWidgetItem();
                item5->setText(query.value("cgst").toString());
                ui->foodOrderList->setItem(row,5,item5);

                QTableWidgetItem *item6 = new QTableWidgetItem();
                item6->setText(QString::number(ui->qty->text().toDouble()*query.value("food_price").toDouble(),'f',2));
                item6->setData(Qt::UserRole,QVariant::fromValue(ui->qty->text().toDouble()*ui->price->text().toDouble()));
                ui->foodOrderList->setItem(row,6,item6);
            }
        }
        ui->foodOrderList->resizeColumnsToContents();

        ui->foodItem->clear();
        ui->price->clear();

        //qDebug()<<row;

    //}
}

void AddFoodOrder::on_saveBtn_clicked()
{
    DbMysql* d = DbMysql::getInstance();
    QMessageBox msgBox;

    QString sql = "insert into food_orders values ";
    bool comaFlag = true;

    if(!d->getConnection().open())
    {
        msgBox.critical(this,"Error","Failed to connect database.1");
    }
    else
    {
        for(int i=0;i<ui->foodOrderList->rowCount();i++)
        {
            if(!comaFlag)sql+=",";

            QString qty = ui->foodOrderList->item(i,2)->data(Qt::UserRole).toString();
            double rate = ui->foodOrderList->item(i,3)->text().toDouble();
            double cgst = ui->foodOrderList->item(i,4)->text().toDouble();
            double sgst = ui->foodOrderList->item(i,5)->text().toDouble();
            QString hsn_code =  ui->foodOrderList->item(i,0)->text();

            sql += "(DEFAULT,'"+ui->foodOrderList->item(i,1)->text()+"',now(),"+qty+","+QString::number(rate,'f',2)+",'"+hsn_code+"',"+QString::number(sgst)+","+QString::number(cgst)+","+QString::number(this->booking_id)+")";
            comaFlag=false;

        }

        qDebug()<<sql;

        QSqlQuery query1( sql,d->getConnection());

        if(!query1.isActive())        {

            msgBox.critical(this,"Error","Failed to connect database.");
        }
        else
        {
            msgBox.information(this,"Success","Food order added.");
            this->close();
        }
    }
}

void AddFoodOrder::on_removeItem_clicked()
{
    QMessageBox msgBox;
    if(ui->foodOrderList->selectedItems().size() > 0){
        ui->foodOrderList->removeRow(ui->foodOrderList->selectedItems().at(0)->row());
    }
    else
    {
        msgBox.information(this,"Remove food items","Please select an item.");
    }
}
