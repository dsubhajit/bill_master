#include "customers.h"
#include "ui_customers.h"
#include <QString>
#include <QStringList>
#include "dbmysql.h"
#include <QMessageBox>
#include "add_new_customer.h"

Customers::Customers(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Customers)
{
    ui->setupUi(this);

}

Customers::~Customers()
{
    delete ui;
}




void Customers::getAllCustomerData()
{
    DbMysql* d = DbMysql::getInstance();
    /*
    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
    db.setHostName("localhost");
    db.setDatabaseName("testQt");
    db.setUserName("root");
    db.setPort(3306);
    db.setPassword("");
    */
    if (!d->getConnection().open()){
        qDebug() << "Failed to connect to root mysql admin";
        qDebug() << d->getConnection().lastError().text();
    }
    else {
    /*
    DbMysql* db1 = DbMysql::getInstance();

    if(!db1->getDb().open()){
        return;
    }
    else {
    */
        QSqlQuery query( "SELECT * FROM customers order by customer_id desc;",d->getConnection() );

        if( !query.isActive() )
        {
            QMessageBox msgBox;
            msgBox.critical(this,"Error","Failed to execute query.");
            qDebug()<<"Failed to execute query. select customers";
            qDebug()<<query.lastError().text();
            return;
        }
        else{
            int row = 0;
            ui->customersDetailsTable->setRowCount(query.size());
            while( query.next() )
            {

                //QStringList ss = line.split('\t');

                //if(ui->customersDetailsTable->rowCount() < row + 1)

                //if(ui->tableWidget->columnCount() < ss.size())
                //    ui->tableWidget->setColumnCount( ss.size() );
                int columnSize = ui->customersDetailsTable->columnCount();
                //qDebug()<<"Column Size:"<<ui->customersDetailsTable->columnCount();

                for( int column = 0; column < columnSize; column++)
                {
                    QTableWidgetItem *newItem = new QTableWidgetItem();
                    newItem->setText(query.value(column).toString());
                    //qDebug()<<"Data:"<<query.value(column).toString();

                    ui->customersDetailsTable->setItem(row, column, newItem);


                }

                row++;
                //int fff = query.value( 0 ).toInt();
            }
            //ui->customersDetailsTable->resizeColumnsToContents();
            ui->customersDetailsTable->horizontalHeader()->setHighlightSections(false);
            //ui->customersDetailsTable->horizontalHeader()->setStyleSheet("QHeaderView::section { background-color:#0B2161;color:#fff; }");
        }
        d->getConnection().close();
    //}

    }
}
/*
void Customers::on_toolButton_triggered(QAction *arg1)
{

}
*/
void Customers::on_addCustomer_clicked()
{
    qDebug()<<"button clicked";
    add_new_customer* addc = new add_new_customer();
    addc->setCustomerParent(this);
    addc->show();

}

void Customers::on_editCustomer_clicked()
{
    if(ui->customersDetailsTable->selectedItems().size() > 0)
    {
        QString customer_id = ui->customersDetailsTable->selectedItems().at(0)->text();
        add_new_customer* adc = new add_new_customer();
        adc->editCustomer(customer_id);
        adc->show();
    }
    else
    {
        QMessageBox msgBox;
        msgBox.information(this,"Edit Customer Details","Please select a customer before edit.");
    }

}

void Customers::on_deleteCustomer_clicked()
{
    QMessageBox::StandardButton reply;
    QMessageBox msgBox;
    reply = QMessageBox::question(this, "Delete Customer", "Are you want to delete customer?",QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        qDebug() << "Yes was clicked";
        DbMysql* d = DbMysql::getInstance();
        QString customer_id = ui->customersDetailsTable->selectedItems().at(0)->text();
        if (!d->getConnection().open()) {
            msgBox.critical(this,"Error","Failed to connect database.");
        }
        else {
            QSqlQuery query("delete from customers where customer_id="+customer_id+";",d->getConnection());

            if( !query.exec() )
            {
                msgBox.critical(this,"Error","Failed to execute query.");
                qDebug()<<query.lastError().text();
                return;
            }
            else{
                msgBox.information(this,"Information","Customer Deleted.");
                this->getAllCustomerData();
            }
        }
    }
}

void Customers::on_searchBtn_clicked()
{
    DbMysql* d = DbMysql::getInstance();

    if (!d->getConnection().open()){
        qDebug() << "Failed to connect to root mysql admin";
        qDebug() << d->getConnection().lastError().text();
    }
    else {

        QSqlQuery query( "SELECT * FROM customers where customer_name like '%"+ui->customerName->text()+"%';",d->getConnection() );

        if( !query.isActive() )
        {
            QMessageBox msgBox;
            msgBox.critical(this,"Error","Failed to execute query.");
            qDebug()<<"Failed to execute query. select customers";
            qDebug()<<query.lastError().text();
            return;
        }
        else{
            int row = 0;
            ui->customersDetailsTable->setRowCount(query.size());
            while( query.next() )
            {
                int columnSize = ui->customersDetailsTable->columnCount();

                for( int column = 0; column < columnSize; column++)
                {
                    QTableWidgetItem *newItem = new QTableWidgetItem();
                    newItem->setText(query.value(column).toString());
                    ui->customersDetailsTable->setItem(row, column, newItem);


                }

                row++;
            }
            ui->customersDetailsTable->resizeColumnsToContents();
            ui->customersDetailsTable->horizontalHeader()->setHighlightSections(false);

        }
        d->getConnection().close();
    }
}
