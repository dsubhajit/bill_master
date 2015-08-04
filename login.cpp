#include "login.h"
#include "ui_login.h"
#include "dbmysql.h"
#include <QMessageBox>
#include <QDebug>
#include <QCryptographicHash>
#include "mainwindow.h"

Login::Login(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Login)
{
    ui->setupUi(this);
}

Login::~Login()
{
    delete ui;
}



void Login::on_loginBtn_clicked()
{

    DbMysql* d = DbMysql::getInstance();
    QMessageBox msgBox;

    QString username = ui->username->text();

    //QString(QCryptographicHash::hash(ui->password->text()),)

    QString password = QString(QCryptographicHash::hash((ui->password->text().toUtf8()),QCryptographicHash::Md5).toHex());

    password = password.toLower();

    if(!d->getConnection().open())
    {
        msgBox.critical(this,"Error","Failed to connect database.1");
    }
    else
    {

        QSqlQuery query1( "select * from administrator where username='"+username+"' and password='"+password+"' ;",d->getConnection());
        qDebug()<<query1.lastQuery();
        if(!query1.isActive())
        {
            msgBox.critical(this,"Error","Failed to connect database.");
        }
        else
        {
            if(query1.size() > 0)
            {

                emit loggedIn();
                this->close();
            }
            else
            {
                msgBox.warning(this,"Login Failed","Please check your username or password.");

            }
        }
    }
}

void Login::on_cancelBtn_clicked()
{
    this->close();
}
