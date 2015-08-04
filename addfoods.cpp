#include "addfoods.h"
#include "ui_addfoods.h"
#include "dbmysql.h"
#include <QMessageBox>

AddFoods::AddFoods(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddFoods)
{
    ui->setupUi(this);
}

AddFoods::~AddFoods()
{
    delete ui;
}

void AddFoods::on_cancelAddFood_clicked()
{
    this->close();
}

void AddFoods::addFoodParent(Foods *parent)
{
    this->m_food = parent;
}

void AddFoods::on_saveBtn_clicked()
{
    QString name = ui->foodName->text();
    QString price = ui->price->text();
    QString type = ui->type->currentText();
    QString category =ui->category->currentText();
    QString desc = ui->desc->toPlainText();

    DbMysql* d = DbMysql::getInstance();
    QMessageBox msgBox;
    if(!d->getConnection().open()){
        msgBox.critical(this,"Error","Failed to connect database.");
    }
    else {
        QSqlQuery query( "insert into foods values(DEFAULT,'"+name+"','"+type+"','"+category+"',"+price+",'"+desc+"');" ,d->getConnection());
        if( !query.isActive() )
        {
            qDebug()<<"Failed to execute query. insert room.";
            qDebug()<<query.lastQuery();
            qDebug()<<query.lastError().text();
            return;
        }
        else
        {
            msgBox.information(this,"Success","New food added.");
            this->m_food->createFoodsTable();
        }
    }


}
