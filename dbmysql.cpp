#include "dbmysql.h"
#include <QFile>
#include <QXmlStreamReader>
#include <QMessageBox>

DbMysql::DbMysql()
{
    qDebug()<<"Path"<<QDir::currentPath();

    QFile file(QDir::currentPath()+"/dbInfo.xml");
    if (!file.open(QFile::ReadOnly | QFile::Text))
    {
        qDebug()<< "Error: Cannot read file " << qPrintable("filename")
         << ": " << qPrintable(file.errorString());

    }



    QXmlStreamReader *xmlReader = new QXmlStreamReader();

    xmlReader->setDevice(&file);
    while(!xmlReader->atEnd() && !xmlReader->hasError()) {
        // Read next element
        QXmlStreamReader::TokenType token = xmlReader->readNext();
        //If token is just StartDocument - go to next
        if(token == QXmlStreamReader::StartDocument) {
                continue;
        }
        //If token is StartElement - read it
        if(token == QXmlStreamReader::StartElement) {

            if(xmlReader->name() == "dbinfo") {
                    continue;
            }

            if(xmlReader->name() == "username") {
                mUserName = xmlReader->readElementText();
            }

            if(xmlReader->name() == "password") {
                mPassword = xmlReader->readElementText();
            }
            if(xmlReader->name() == "port") {
                mPort = xmlReader->readElementText().toInt();
            }
            if(xmlReader->name() == "host") {
                mHostName =  xmlReader->readElementText();
            }
            if(xmlReader->name() == "database") {
                mDb = xmlReader->readElementText();
            }
        }
    }

    qDebug()<<mUserName<<" " <<mPassword << " " <<mDb << " " <<mHostName << " " << mPort;


    //close reader and flush file
    xmlReader->clear();
    file.close();
    /*
    mHostName = "127.0.0.1";
    mUserName = "root";
    mPort = 3306;
    mPassword = "";
    mDb = "testqt";
    */
    mDbConn = QSqlDatabase::addDatabase("QMYSQL");
    mDbConn.setHostName(mHostName);
    mDbConn.setPassword(mPassword);
    mDbConn.setDatabaseName(mDb);
    mDbConn.setUserName(mUserName);
    mDbConn.setPort(mPort);
    if(!mDbConn.open())
    {
        qDebug()<<mDbConn.lastError();
        exit(1);
    }
}

DbMysql* DbMysql::mThis = NULL;

DbMysql* DbMysql::getInstance()
{
    if(mThis!=NULL)
    {
        return mThis;
    }
    else {
        mThis = new DbMysql();
        return mThis;
    }
}

QSqlDatabase DbMysql::getConnection()
{
    return mDbConn;
}

QString DbMysql::getDb()
{
    return mDb;
}

QString DbMysql::getHost()
{
    return mHostName;
}

QString DbMysql::getPassWord()
{
    return mPassword;
}

QString DbMysql::getUserName()
{
    return mUserName;
}

int DbMysql::getPort()
{
    return mPort;
}

DbMysql::~DbMysql()
{

}


