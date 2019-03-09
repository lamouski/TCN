#-------------------------------------------------
#
# Project created by QtCreator 2018-04-21T21:56:13
#
#-------------------------------------------------

QT       += network core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets sql printsupport

TARGET = TCN2020
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
        mainwindow.cpp \
    dbmanager.cpp \
    memberslistdialog.cpp \
    weekviewwidget.cpp \
    daybookingtablemodel.cpp \
    fieldlistdialog.cpp \
    bookingdialog.cpp \
    dayreportwidget.cpp \
    documentwidget.cpp \
    fielddelegate.cpp \
    fieldstablemodel.cpp \
    pricelistdialog.cpp \
    texteditor.cpp \
    weekreportwidget.cpp \
    servicedialog.cpp \
    settings.cpp \
    pricestablemodel.cpp \
    kassaviewwidget.cpp \
    daykassatablemodel.cpp \
    fieldselectiondialog.cpp \
    logindatadialog.cpp \
    simplecrypt.cpp \
    blockbookingsview.cpp \
    blockbookingsmodel.cpp

HEADERS += \
        mainwindow.h \
    dbmanager.h \
    memberslistdialog.h \
    weekviewwidget.h \
    daybookingtablemodel.h \
    fieldlistdialog.h \
    bookingdialog.h \
    dayreportwidget.h \
    documentwidget.h \
    fielddelegate.h \
    fieldstablemodel.h \
    pricelistdialog.h \
    texteditor.h \
    weekreportwidget.h \
    servicedialog.h \
    settings.h \
    pricestablemodel.h \
    bookingdata.h \
    kassaviewwidget.h \
    daykassatablemodel.h \
    fieldselectiondialog.h \
    logindatadialog.h \
    simplecrypt.h \
    blockbookingsview.h \
    blockbookingsmodel.h

FORMS += \
        mainwindow.ui \
    memberslistdialog.ui \
    weekviewwidget.ui \
    fieldlistdialog.ui \
    bookingdialog.ui \
    dayreportwidget.ui \
    pricelistdialog.ui \
    texteditor.ui \
    weekreportwidget.ui \
    servicedialog.ui \
    kassaviewwidget.ui \
    fieldselectiondialog.ui \
    logindatadialog.ui \
    blockbookingsview.ui

RESOURCES += \
    tcn2020.qrc

RC_FILE = TCN2020.rc

TRANSLATIONS = \
    languages/Translation_en.ts  \
    languages/Translation_de.ts
