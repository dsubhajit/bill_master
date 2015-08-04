#include "mainwindow.h"
#include <QtCore>
#include <QObject>
#include <QtGui>
#include <QApplication>
#include "login.h"
#include <QDebug>
#include "dbmysql.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    //qDebug()<<"yoyo";
    DbMysql* d = DbMysql::getInstance();
    //MainWindow w;

    Login lmain;
    MainWindow mw;

    QObject::connect(&lmain,SIGNAL(loggedIn()),&mw,SLOT(showMaximized()));
    lmain.show();


    //mw.showFullScreen();

    //mw.showMaximized();

    return a.exec();
}
