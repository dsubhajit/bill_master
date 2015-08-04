#include "accounts.h"
#include "ui_accounts.h"
#include "dbmysql.h"
#include <QMessageBox>
#include <QtPrintSupport/QPrinter>
#include <QTextDocument>
#include <QtPrintSupport/QPrintDialog>
#include <QMessageBox>
#include <QPainter>
#include <QWebView>
#include <QPrintDialog>
#include <QTextCursor>
#include <QMargins>
#include <QTextBlockFormat>
#include <QDebug>
#include <QScrollBar>

#include <QDate>
Accounts::Accounts(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Accounts)
{
    ui->setupUi(this);
    QStringList months;

    ui->expTime->setDateTime(QDateTime::currentDateTime());

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

    this->createIncomeTable();
    this->createExpendatureTable();
}

Accounts::~Accounts()
{
    delete ui;
}

void Accounts::createIncomeTable()
{
    int year = ui->yearList->currentData(Qt::UserRole).toInt();
    int month = ui->monthList->currentData(Qt::UserRole).toInt();
    double totalIncome = 0;

    DbMysql* d = DbMysql::getInstance();
    QMessageBox msgBox;

    if(!d->getConnection().open())
    {
        msgBox.critical(this,"Error","Failed to connect database.1");
        return ;
    }
    else
    {

        QString curDate = QDate::currentDate().toString("yyyy-MM-dd");

        QSqlQuery query1( "SELECT inv_number,inv_date,lodging_bill_total,fooding_bill_total,(lodging_bill_total+fooding_bill_total) as amount FROM invoice where MONTH(inv_date)="+QString::number(month)+" and YEAR(inv_date) = "+QString::number(year)+" and status=0 order by inv_id desc;" ,d->getConnection());
        if(!query1.isActive())
        {
            msgBox.critical(this,"Error","Failed to connect database.");
            qDebug()<<query1.lastQuery();
            return ;
        }
        else
        {
            ui->incomeTable->setRowCount(query1.size());
            int row=0;
            while(query1.next())
            {
                for(int i=0;i<ui->incomeTable->columnCount();i++)
                {
                    QTableWidgetItem* item = new QTableWidgetItem();
                    item->setText(query1.value(i).toString());
                    item->setData(Qt::UserRole,QVariant::fromValue(query1.value(0).toInt()));
                    ui->incomeTable->setItem(row,i,item);
                }
                totalIncome+=query1.value("amount").toDouble();

                row++;
            }
            ui->incomeTable->resizeColumnsToContents();
        }
    }
    ui->totalIncome->setText(QString::number(totalIncome,'f',2));

}

void Accounts::createExpendatureTable()
{
    int year = ui->yearList->currentData(Qt::UserRole).toInt();
    int month = ui->monthList->currentData(Qt::UserRole).toInt();

    double totalExp = 0;

    DbMysql* d = DbMysql::getInstance();
    QMessageBox msgBox;

    if(!d->getConnection().open())
    {
        msgBox.critical(this,"Error","Failed to connect database.1");
        return ;
    }
    else
    {

        QString curDate = QDate::currentDate().toString("yyyy-MM-dd");

        QSqlQuery query1( "SELECT * FROM expenditures where MONTH(exp_date)="+QString::number(month)+" and YEAR(exp_date) = "+QString::number(year)+" order by exp_id desc;" ,d->getConnection());
        if(!query1.isActive())
        {
            msgBox.critical(this,"Error","Failed to connect database.");
            qDebug()<<query1.lastQuery();
            return ;
        }
        else
        {
            ui->expenditureTable->setRowCount(query1.size());
            int row=0;
            while(query1.next())
            {
                for(int i=1;i<=ui->expenditureTable->columnCount();i++)
                {
                    QTableWidgetItem* item = new QTableWidgetItem();
                    item->setText(query1.value(i).toString());
                    item->setData(Qt::UserRole,QVariant::fromValue(query1.value("exp_id").toInt()));
                    ui->expenditureTable->setItem(row,i-1,item);
                }
                totalExp+=query1.value("exp_amount").toDouble();

                row++;
            }
            ui->expenditureTable->resizeColumnsToContents();
        }
    }
    ui->totalExpenditure->setText(QString::number(totalExp,'f',2));

}

void Accounts::on_addExpBtn_clicked()
{
    DbMysql* d = DbMysql::getInstance();
    QMessageBox msgBox;

    if(ui->expDetails->text().length()>0 && ui->expAmount->text().length() > 0)
    {
        if(!d->getConnection().open())
        {
            msgBox.critical(this,"Error","Failed to connect database.1");
            return ;
        }
        else
        {

            QString curDate = QDate::currentDate().toString("yyyy-MM-dd");

            QSqlQuery query1( "insert into expenditures values (DEFAULT,'"+ui->expTime->dateTime().toString("yyyy-MM-dd hh:mm:ss")+"','"+ui->expDetails->text()+"',"+ui->expAmount->text()+");" ,d->getConnection());
            if(!query1.isActive())
            {
                msgBox.critical(this,"Error","Failed to connect database.");
                qDebug()<<query1.lastQuery();
                return ;
            }
            else
            {
                msgBox.information(this,"Add Expenditure","Expenditure added successfully!");
                ui->expDetails->setText("");
                ui->expAmount->setValue(0.00);
                this->createExpendatureTable();
            }
        }
    }
    else
        msgBox.warning(this,"Add Expenditure","Please fill the fields.");

}

void Accounts::on_removeExpBtn_clicked()
{
    DbMysql* d = DbMysql::getInstance();
    QMessageBox msgBox;
    if(ui->expenditureTable->selectedItems().size() > 0)
    {
        if(!d->getConnection().open())
        {
            msgBox.critical(this,"Error","Failed to connect database.1");
            return ;
        }
        else
        {

            QString curDate = QDate::currentDate().toString("yyyy-MM-dd");

            QSqlQuery query1( "delete from expenditures where exp_id="+ui->expenditureTable->selectedItems().at(0)->data(Qt::UserRole).toString()+";" ,d->getConnection());
            if(!query1.isActive())
            {
                msgBox.critical(this,"Error","Failed to connect database.");
                qDebug()<<query1.lastQuery();
                return ;
            }
            else
            {
                msgBox.information(this,"Remove Expenditure","Expenditure removed successfully!");
                this->createExpendatureTable();
            }
        }
    }
    else msgBox.information(this,"Remove Expenditure","Please select an expenditure from table.");
}



void Accounts::on_totalIncome_textChanged(const QString &arg1)
{
    ui->balance->setText(QString::number(ui->totalIncome->text().toDouble()-ui->totalExpenditure->text().toDouble(),'f',2));
}

void Accounts::on_totalExpenditure_textChanged(const QString &arg1)
{
    ui->balance->setText(QString::number(ui->totalIncome->text().toDouble()-ui->totalExpenditure->text().toDouble(),'f',2));
}

void Accounts::on_expFilter_clicked()
{
    this->createIncomeTable();
    this->createExpendatureTable();

}

void Accounts::on_printButton_clicked()
{
    QMessageBox msgBox;
    DbMysql* d = DbMysql::getInstance();
    int year = ui->yearList->currentData(Qt::UserRole).toInt();
    int month = ui->monthList->currentData(Qt::UserRole).toInt();

    QTextDocument *document = new QTextDocument();
    QString html = "<!DOCTYPE html><html><head><title>Invoice</title><meta http-equiv=\"Content-Type\" content =\"text/html;charset=utf-8\" ><style type=\"text/css\" >table tr td { font-size:12px; } table { border-collapse: collapse; }</style></head>";
    html+= "<body style=\"color:#000;width:1024px;\" >";
    html+= "<h4 align='center'>Income Expenditure Statement</h4><br />";
    QDate dt(year,month,1);

    html+= "<p>For "+dt.toString("MMMM yyyy")+"</p>";
    html+= "<table width='100%' cellpadding='3' >";
    html+= "<tr><td align='center' >Income</td><td align='center' >Expenditure</td></tr>";
    html+="<tr><td>";
    html+="<table width='100%' border=1 cellpadding='3' >";
    html+="<tr align='center' style='background:#efefef;' ><td>Invoice Number</td><td>Date</td><td>Total Amount</td></tr>";
    int i =0;
    double total_amount_income = 0;

    if(!d->getConnection().open())
    {
        msgBox.critical(this,"Error","Failed to connect database.1");
        return ;
    }
    else
    {

        QString curDate = QDate::currentDate().toString("yyyy-MM-dd");

        QSqlQuery query1( "SELECT inv_number,inv_date,lodging_bill_total,fooding_bill_total,(lodging_bill_total+fooding_bill_total) as amount FROM invoice where MONTH(inv_date)="+QString::number(month)+" and YEAR(inv_date) = "+QString::number(year)+" and status=0 order by inv_id desc;" ,d->getConnection());
        if(!query1.isActive())
        {
            msgBox.critical(this,"Error","Failed to connect database.");
            qDebug()<<query1.lastQuery();
            return ;
        }
        else
        {

            while(query1.next())
            {
                html+="<tr align='center' ><td>"+query1.value("inv_number").toString()+"</td><td>"+query1.value("inv_date").toDateTime().toString("dd-mm-yyyy")+"</td><td align='right' >"+QString::number(query1.value("amount").toDouble(),'f',2)+"</td></tr>";
                total_amount_income += query1.value("amount").toDouble();
            }
        }
    }

    html+= "<tr><td colspan='2' align='right' >Total</td><td align='right' >"+QString::number(total_amount_income,'f',2)+"</td></tr>";

    html+="</table>";
    html+="</td><td>";
    html+="<table width='100%' border=1 cellpadding='3' >";
    html+="<tr align='center' style='background:#efefef;' ><td>Details</td><td>Date</td><td>Total Amount</td></tr>";
    double total_exp_amount = 0;
    if(!d->getConnection().open())
    {
        msgBox.critical(this,"Error","Failed to connect database.1");
        return ;
    }
    else
    {

        QSqlQuery query1( "SELECT * FROM expenditures where MONTH(exp_date)="+QString::number(month)+" and YEAR(exp_date) = "+QString::number(year)+" order by exp_id desc;" ,d->getConnection());
        if(!query1.isActive())
        {
            msgBox.critical(this,"Error","Failed to connect database.");
            qDebug()<<query1.lastQuery();
            return ;
        }
        else
        {
            while(query1.next())
            {
                html+="<tr align='center' ><td>"+query1.value("exp_details").toString()+"</td><td>"+query1.value("exp_date").toString()+"</td><td align='right' >"+QString::number(query1.value("exp_amount").toDouble(),'f',2)+"</td></tr>";
                total_exp_amount+=query1.value("exp_amount").toDouble();
            }
        }
    }
    html+= "<tr><td colspan='2' align='right' >Total</td><td align='right' >"+QString::number(total_exp_amount,'f',2)+"</td></tr>";
    html+="</table>";
    html+="</td></tr>";
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
