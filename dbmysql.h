#include <QtSql>
#include <QString>

#ifndef DBMYSQL_H
#define DBMYSQL_H


class DbMysql
{

private:
    DbMysql();
    QString mHostName;
    QString mUserName;
    QString mPassword;
    QString mDb;
    int  mPort;

    QSqlDatabase mDbConn;

    static DbMysql* mThis;

public:
    static DbMysql* getInstance();
    QString getDb();
    QString getUserName();
    QString getPassWord();
    QString getHost();

    QSqlDatabase getConnection();

    int getPort();

    ~DbMysql();
};

#endif // DBMYSQL_H
