#include <QtCore>
#include <QtGui>
#include <QLayout>
#ifndef APPUTILS_H
#define APPUTILS_H


class AppUtils
{
public:
    AppUtils();
    static void clearLayout(QLayout* layout, bool deleteWidgets, QString space);
    static QString  numberToText(uint number);
    ~AppUtils();
};

#endif // APPUTILS_H
