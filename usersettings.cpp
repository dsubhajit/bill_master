#include "usersettings.h"
#include "ui_usersettings.h"
#include "dbmysql.h"
#include <QMessageBox>
#include <QCryptographicHash>

UserSettings::UserSettings(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::UserSettings)
{
    ui->setupUi(this);
}

UserSettings::~UserSettings()
{
    delete ui;
}

void UserSettings::on_saveBtn_clicked()
{
    DbMysql* d = DbMysql::getInstance();
    QMessageBox msgBox;

    if(!d->getConnection().open())
    {
        msgBox.critical(this,"Error","Failed to connect database.1");
    }
    else
    {
        QString password = QString(QCryptographicHash::hash((ui->oldPass->text().toUtf8()),QCryptographicHash::Md5).toHex());

        QSqlQuery query1( "select * from administrator where username='admin' and password='"+password.toLower()+"';" ,d->getConnection());
        if(!query1.isActive())
        {
            qDebug()<<query1.lastQuery();
            msgBox.critical(this,"Error","Failed to connect database.");
        }
        else
        {
            if(query1.size() > 0)
            {
                if(ui->newPass->text().length() > 0 && ui->newPass->text()==ui->reNewPass->text())
                {
                    QString newPass = QString(QCryptographicHash::hash((ui->newPass->text().toUtf8()),QCryptographicHash::Md5).toHex());
                    QSqlQuery query2( "update administrator set password='"+newPass+"' where username='admin' and password='"+password.toLower()+"';" ,d->getConnection());
                    if(!query1.isActive())
                    {
                        qDebug()<<query1.lastQuery();
                        msgBox.critical(this,"Error","Failed to connect database.");
                    }
                    else
                    {
                        msgBox.information(this,"Change Password","Password changed successfully.");
                        this->close();
                    }
                }
                else msgBox.warning(this,"Change Password","Please enter valid passwords.");
            }
            else msgBox.warning(this,"Change Password","Old password not matched.");
        }
    }
}
