#include "gstinfo.h"
#include "ui_gstinfo.h"
#include <QTreeWidgetItem>
#include <QDate>
#include <QSqlQuery>
#include <QMessageBox>
#include <QTableWidgetItem>
#include "dbmysql.h"
#include <QColor>
#include <QFont>

GstInfo::GstInfo(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GstInfo)
{
    ui->setupUi(this);
    int cur_year= QDate::currentDate().year();

    for(int i=2000;i<2050;i++)
    {
        ui->yearList->addItem(QString::number(i),QVariant::fromValue(i));
        //qDebug()<<i;
    }
    ui->yearList->setCurrentIndex(cur_year-2000);
    createYearlyTree();
    populateData(ui->yearList->currentText().toInt());
}

GstInfo::~GstInfo()
{
    delete ui;




}


void GstInfo::createYearlyTree()
{
    ui->treeMonths->setColumnCount(1);
    ui->treeMonths->setHeaderLabel("Year");

    QTreeWidgetItem *item = new QTreeWidgetItem();

    item->setText(0,ui->yearList->currentText());
    item->setData(0,Qt::UserRole,QVariant::fromValue(ui->yearList->currentData(Qt::UserRole)));    
    ui->treeMonths->addTopLevelItem(item);

    ui->treeMonths->topLevelItem(0)->setSelected(true);


    QStringList months;

    months<<"Jan"<<"Feb"<<"Mar"<<"Apr"<<"May"<<"Jun"<<"Jul"<<"Aug"<<"Sep"<<"Oct"<<"Nov"<<"Dec";
    int i=1;
    foreach (QString mnth, months) {
        QTreeWidgetItem *itm = new QTreeWidgetItem();
        itm->setText(0,mnth);
        itm->setData(0,Qt::UserRole,QVariant::fromValue(i));
        item->addChild(itm);
        i++;
    }
}

void GstInfo::populateData(int aYear)
{
    int data = aYear;
    DbMysql* d = DbMysql::getInstance();
    QMessageBox msgBox;


   double cgst_val = 0;
   double sgst_val = 0;
   QStringList hLabels;
   hLabels<<"Month"<<"Collected CGST" << "Collected SGST" << "Total GST";

   ui->gstDataTable->setColumnCount(4);
   ui->gstDataTable->setHorizontalHeaderLabels(hLabels);
   ui->gstDataTable->horizontalHeader()->setStyleSheet("QHeaderView::section { background-color:#0B2161;color:#fff; }");


   QSqlQuery query1( "SELECT  m, SUM(gst_data.cgst_amount) as cgst_amount, SUM(gst_data.sgst_amount) as sgst_amount FROM (SELECT y, m FROM (SELECT "+QString::number(data)+" y ) years, (SELECT 1 m UNION ALL SELECT 2 UNION ALL SELECT 3 UNION ALL SELECT 4 UNION ALL SELECT 5 UNION ALL SELECT 6 UNION ALL SELECT 7 UNION ALL SELECT 8 UNION ALL SELECT 9 UNION ALL SELECT 10 UNION ALL SELECT 11 UNION ALL SELECT 12) months ) ym  LEFT JOIN gst_data on ym.y = YEAR(gst_data.inv_date) and ym.m = MONTH(gst_data.inv_date)  group by m;" ,d->getConnection());
   if(!query1.isActive())
   {
       msgBox.critical(this,"Error","Failed to connect database.");
       qDebug()<<query1.lastQuery();
       return ;
   }
   else
   {
       qDebug()<<query1.lastQuery();
       ui->gstDataTable->setRowCount(query1.size());

       QStringList months;
       months<<"January"<<"February"<<"March"<<"April"<<"May"<<"June"<<"July"<<"August"<<"September"<<"October"<<"November"<<"December";
       int i=0;
       while(query1.next())
       {
           QTableWidgetItem *itm1 = new QTableWidgetItem();
           itm1->setText(months.at(query1.value(0).toInt() - 1));
           itm1->setData(Qt::UserRole,QVariant::fromValue(query1.value(0).toInt()));
           itm1->setTextAlignment(Qt::AlignCenter);
           ui->gstDataTable->setItem(i,0,itm1);

           QTableWidgetItem *itm2 = new QTableWidgetItem();
           itm2->setText(QString::number(query1.value(1).toDouble(),'f',2));
           itm2->setData(Qt::UserRole,QVariant::fromValue(query1.value(0).toInt()));
           itm2->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
           ui->gstDataTable->setItem(i,1,itm2);

           cgst_val+= query1.value(1).toDouble();

           QTableWidgetItem *itm3 = new QTableWidgetItem();
           itm3->setText(QString::number(query1.value(2).toDouble(),'f',2));
           itm3->setData(Qt::UserRole,QVariant::fromValue(query1.value(0).toInt()));
           itm3->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);

           sgst_val+= query1.value(2).toDouble();

           ui->gstDataTable->setItem(i,2,itm3);

           QTableWidgetItem *itm4 = new QTableWidgetItem();
           itm4->setText(QString::number(query1.value(2).toDouble()+query1.value(1).toDouble(),'f',2));
           itm4->setData(Qt::UserRole,QVariant::fromValue(query1.value(0).toInt()));
           itm4->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);

           QColor clr;
           clr.setNamedColor("#FFF59D");

           itm4->setBackgroundColor(clr);
           QFont font;
           font.setBold(true);
           itm4->setFont(font);
           ui->gstDataTable->setItem(i++,3,itm4);

       }
   }
   ui->collectedCgst->setText(QString::number(cgst_val,'f',2));
   ui->collectedSgst->setText(QString::number(sgst_val,'f',2));
   ui->totalGst->setText(QString::number(cgst_val+sgst_val,'f',2));

   ui->gstInfoText->setText("GST Information for the year");
   ui->gstInfoLabel->setText(" ( "+ ui->yearList->currentText()+ " )");
}

void GstInfo::on_treeMonths_itemDoubleClicked(QTreeWidgetItem *item, int column)
{
    int data = item->data(column,Qt::UserRole).toInt();
    DbMysql* d = DbMysql::getInstance();
    QMessageBox msgBox;

    if(data > 2000)
    {
        populateData(data);
    }
    else
    {
        double cgst_val = 0;
        double sgst_val = 0;

        QStringList months;
        months<<"January"<<"February"<<"March"<<"April"<<"May"<<"June"<<"July"<<"August"<<"September"<<"October"<<"November"<<"December";

        QStringList hLabels;
        hLabels<<"Invoice No."<<"Invoice Date"<<"Booking No."<<"Collected CGST" << "Collected SGST" << "Total GST";

        ui->gstDataTable->setColumnCount(hLabels.length());
        ui->gstDataTable->setHorizontalHeaderLabels(hLabels);
        ui->gstDataTable->horizontalHeader()->setStyleSheet("QHeaderView::section { background-color:#0B2161;color:#fff; }");


        QSqlQuery query1("select * from gst_data where YEAR(inv_date)="+ui->yearList->currentText()+" and MONTH(inv_date)="+QString::number(data) ,d->getConnection());
        if(!query1.isActive())
        {
           msgBox.critical(this,"Error","Failed to connect database.");
           qDebug()<<query1.lastQuery();
           return ;
        }
        else
        {
           qDebug()<<query1.lastQuery();
           ui->gstDataTable->setRowCount(query1.size());


           int i=0;
           while(query1.next())
           {
               QTableWidgetItem *itm1 = new QTableWidgetItem();
               itm1->setText(query1.value("inv_number").toString());
               //itm1->setData(Qt::UserRole,QVariant::fromValue(query1.value("inv_no").toInt()));
               itm1->setTextAlignment(Qt::AlignCenter);
               ui->gstDataTable->setItem(i,0,itm1);

               QTableWidgetItem *itm2 = new QTableWidgetItem();
               itm2->setText(query1.value("inv_date").toString());
               //itm2->setData(Qt::UserRole,QVariant::fromValue(query1.value(0).toInt()));
               itm2->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
               ui->gstDataTable->setItem(i,1,itm2);

               cgst_val+= query1.value("cgst_amount").toDouble();

               QTableWidgetItem *itm3 = new QTableWidgetItem();
               itm3->setText(query1.value("booking_id").toString());
               itm3->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);

               sgst_val+= query1.value("sgst_amount").toDouble();

               ui->gstDataTable->setItem(i,2,itm3);

               QTableWidgetItem *itm4 = new QTableWidgetItem();
               itm4->setText(QString::number(query1.value("cgst_amount").toDouble(),'f',2));
               itm4->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
               ui->gstDataTable->setItem(i,3,itm4);

               QTableWidgetItem *itm5 = new QTableWidgetItem();
               itm5->setText(QString::number(query1.value("sgst_amount").toDouble(),'f',2));
               itm5->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter );
               ui->gstDataTable->setItem(i,4,itm5);

               QTableWidgetItem *itm6 = new QTableWidgetItem();
               itm6->setText(QString::number(query1.value("cgst_amount").toDouble()+query1.value("sgst_amount").toDouble(),'f',2));
               itm6->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);

               QColor clr;
               clr.setNamedColor("#FFF59D");
               itm6->setBackgroundColor(clr);
               QFont font;
               font.setBold(true);
               itm6->setFont(font);

               ui->gstDataTable->setItem(i++,5,itm6);





           }
        }
        ui->collectedCgst->setText(QString::number(cgst_val,'f',2));
        ui->collectedSgst->setText(QString::number(sgst_val,'f',2));
        ui->totalGst->setText(QString::number(cgst_val+sgst_val,'f',2));

        ui->gstInfoText->setText("GST Information for the month");
        ui->gstInfoLabel->setText(" ( "+months.at(data-1)+ " ) ");
    }

}

void GstInfo::on_printGstReport_clicked()
{

}
