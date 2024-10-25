#include "addfoods.h"
#include "ui_addfoods.h"
#include "dbmysql.h"
#include <QMessageBox>
#include <QListWidgetItem>

AddFoods::AddFoods(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddFoods)
{
    ui->setupUi(this);
    this->mEditMode = false;
    this->mFoodId = 0;
}

AddFoods::~AddFoods()
{
    delete ui;
}

void AddFoods::setEditMode(bool aEditMode)
{
    this->mEditMode = aEditMode;
    if(this->mEditMode)
    {
        DbMysql* d = DbMysql::getInstance();
        QMessageBox msgBox;
        if(!d->getConnection().open()){
            msgBox.critical(this,"Error","Failed to connect database.");
        }
        else {
            QSqlQuery query( "select * from foods where food_id="+QString::number(this->mFoodId) ,d->getConnection());
            qDebug()<<"select * from foods where food_id="+QString::number(this->mFoodId);
            if( query.isActive() )
            {
                query.next();
                qDebug()<<query.value(0).toString();
                ui->foodName->setText(query.value("food_name").toString());
                ui->foodCgst->setValue(query.value("cgst").toDouble());
                ui->foodSgst->setValue(query.value("sgst").toDouble());
                ui->foodHsnCode->setText(query.value("hsn_code").toString());
                ui->price->setValue(query.value("food_price").toDouble());
                ui->desc->setText(query.value("food_desc").toString());



                for(int i = 0; i < ui->type->count(); ++i)
                {
                    QString item = ui->type->itemText(i);
                    if(QString::compare(item,query.value("food_type").toString()) == 0)
                    {
                        ui->type->setCurrentIndex(i);
                    }
                }
                for(int i = 0; i < ui->category->count(); ++i)
                {
                    QString item = ui->category->itemText(i);
                    if(QString::compare(item,query.value("food_category").toString()) == 0)
                    {
                        ui->category->setCurrentIndex(i);
                    }
                }
            }


        }
    }
}

void AddFoods::on_cancelAddFood_clicked()
{
    this->close();
}

void AddFoods::setFoodId(int aFoodId)
{
    this->mFoodId = aFoodId;
    qDebug()<<"Food Id"<<this->mFoodId;
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
    QString hsn_code = ui->foodHsnCode->text();
    QString cgst = ui->foodCgst->text();
    QString sgst = ui->foodSgst->text();


    DbMysql* d = DbMysql::getInstance();
    QMessageBox msgBox;
    if(!d->getConnection().open()){
        msgBox.critical(this,"Error","Failed to connect database.");
    }
    else {
        QString sql;
        if(this->mEditMode)
        {
            sql = "update foods set food_name='"+name+"',food_type='"+type+"',food_category='"+category+"',food_price="+price+",food_desc='"+desc+"',hsn_code='"+hsn_code+"',sgst="+sgst+",cgst="+cgst+" where food_id="+QString::number(this->mFoodId)+";";

        }
        else {
            sql = "insert into foods values(DEFAULT,'"+name+"','"+type+"','"+category+"',"+price+",'"+desc+"','"+hsn_code+"',"+sgst+","+cgst+");";
        }

        QSqlQuery query(sql,d->getConnection());

        if( !query.isActive() )
        {
            qDebug()<<"Failed to execute query. insert food.";
            qDebug()<<query.lastQuery();
            qDebug()<<query.lastError().text();
            return;
        }
        else
        {
            if(this->mEditMode)
            {
                msgBox.information(this,"Success","Food Updated Successfully.");
            }
            else {
                msgBox.information(this,"Success","New food added.");
            }
            this->close();
            this->m_food->createFoodsTable();
        }
    }


}
