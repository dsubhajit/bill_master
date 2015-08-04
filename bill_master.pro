#-------------------------------------------------
#
# Project created by QtCreator 2015-04-08T19:43:36
#
#-------------------------------------------------

QT       += core gui
QT       += sql printsupport webkitwidgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = bill_master
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    customers.cpp \
    rooms.cpp \
    dbmysql.cpp \
    add_new_customer.cpp \
    addrooms.cpp \
    addnewequipments.cpp \
    bookingcalendar.cpp \
    foods.cpp \
    addfoods.cpp \
    createbooking.cpp \
    apputils.cpp \
    createlog.cpp \
    logbook.cpp \
    invoices.cpp \
    createinvoices.cpp \
    home.cpp \
    roomsettings.cpp \
    addfoodorder.cpp \
    aboutbillmaster.cpp \
    login.cpp \
    hotelinfo.cpp \
    qcustomplot.cpp \
    stats.cpp \
    accounts.cpp \
    usersettings.cpp \
    addadvancepayment.cpp


HEADERS  += mainwindow.h \
    customers.h \
    rooms.h \
    dbmysql.h \
    add_new_customer.h \
    addrooms.h \
    addnewequipments.h \
    bookingcalendar.h \
    foods.h \
    addfoods.h \
    createbooking.h \
    apputils.h \
    createlog.h \
    logbook.h \
    invoices.h \
    createinvoices.h \
    home.h \
    roomsettings.h \
    addfoodorder.h \
    aboutbillmaster.h \
    login.h \
    hotelinfo.h \
    qcustomplot.h \
    stats.h \
    accounts.h \
    usersettings.h \
    addadvancepayment.h


FORMS    += mainwindow.ui \
    customers.ui \
    rooms.ui \
    add_new_customer.ui \
    addrooms.ui \
    addnewequipments.ui \
    bookingcalendar.ui \
    foods.ui \
    addfoods.ui \
    createbooking.ui \
    createlog.ui \
    logbook.ui \
    invoices.ui \
    createinvoices.ui \
    home.ui \
    roomsettings.ui \
    addfoodorder.ui \
    invoiceprint.ui \
    aboutbillmaster.ui \
    login.ui \
    hotelinfo.ui \
    stats.ui \
    accounts.ui \
    usersettings.ui \
    addadvancepayment.ui

RESOURCES += \
    bill_master_resources.qrc

DISTFILES += \
    db.sql \
    dbInfo.xml

RC_FILE = billmaster.rc
