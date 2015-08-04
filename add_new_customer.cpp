#include "add_new_customer.h"
#include "ui_add_new_customer.h"
#include "customers.h"
#include <QMessageBox>
#include "createbooking.h"
#include <QDebug>
#include "dbmysql.h"

add_new_customer::add_new_customer(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::add_new_customer)
{
    ui->setupUi(this);
    this->mIsCb = false;
}

add_new_customer::~add_new_customer()
{
    delete ui;
}

void add_new_customer::setBookingCreateParent(CreateBooking *Obj)
{
    this->m_cb=Obj;
    mIsCb=true;
}

void add_new_customer::on_cancelAddNewCustomertn_clicked()
{
   this->close();
}

void add_new_customer::on_saveNewCustomer_clicked()
{

    QString name = ui->customer_name->text();
    QString address = ui->address->toPlainText();
    QString phone = ui->phone->text();
    QString email = ui->email->text();

    QString id_type = ui->id_type->currentText();
    QString id_serial = ui->id_serial->text();        
    QString comments = ui->comments->toPlainText();
    QMessageBox msgBox;
    qDebug()<<"Yo:"<<name.length();
    if(name.length()==0){
        msgBox.critical(this,"Error","Please eneter a valid customer name.");
        return;
    }

    DbMysql* d = DbMysql::getInstance();

    if (!d->getConnection().open()) {
        msgBox.critical(this,"Error","Failed to connect database.");
    }
    else {
        QSqlQuery query( "insert into customers values(DEFAULT,'"+name+"','"+email+"','"+address+"','"+phone+"','"+id_type+"','"+id_serial+"','"+comments+"');" ,d->getConnection());

        if( !query.isActive() )
        {
            qDebug()<<"Failed to execute query. inset customers.";
            qDebug()<<query.lastError().text();
            return;
        }
        else{
            //qDebug()<<query.lastQuery();
            msgBox.information(this,"Add new customer","New customer added.");
            if(this->mIsCb)
            {
                this->m_cb->addCustomersInForm();
            }
            else
            {
                this->mCustomer->getAllCustomerData();
            }
            this->close();            
        }
    }
    d->getConnection().close();
}

void add_new_customer::setCustomerParent(Customers *parent)
{
    this->mCustomer= parent;
}

void add_new_customer::editCustomer(QString aCustomerId)
{

    QMessageBox msgBox;
    DbMysql* d = DbMysql::getInstance();
    /*
    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
    db.setHostName(d->getHost());
    db.setDatabaseName(d->getDb());
    db.setUserName(d->getUserName());
    db.setPort(d->getPort());
    db.setPassword(d->getPassWord());
    */
    if (!d->getConnection().open()) {
        qDebug()<<d->getConnection().lastError();
        msgBox.critical(this,"Error","Failed to connect database.");
    }
    else {
        QSqlQuery query("select * from customers where customer_id="+aCustomerId+";",d->getConnection());
        //query.prepare( );

        //query.bindValue(":first",aCustomerId);
        qDebug()<<query.lastQuery();
        if( !query.isActive() )
        {
            msgBox.critical(this,"Error","Failed to execute query.");
            qDebug()<<query.lastError().text();
            return;
        }
        else{
            if( query.size() > 0 )
            {   query.first();
                /*
                qDebug()<<"Size:"<<query.size();
                qDebug()<<query.value("customer_name").toString();
                qDebug()<<query.lastError();
                */

                this->setWindowTitle("Edit Customer ( "+query.value("customer_name").toString()+" )");
                ui->customer_name->setText(query.value("customer_name").toString());
                ui->address->setText(query.value("address").toString());
                ui->email->setText(query.value("email").toString());
                ui->phone->setText(query.value("phone").toString());
                ui->comments->setPlainText(query.value("comments").toString());

                ui->id_type->setCurrentText(query.value("id_type").toString());
                ui->id_serial->setText(query.value("id_serial").toString());

                d->getConnection().close();
            }
        }
    }

}
