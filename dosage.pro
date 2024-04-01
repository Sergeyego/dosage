QT       += core gui sql network xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

include(xlsx/qtxlsx.pri)

SOURCES += \
    db/dbcombobox.cpp \
    db/dbdateedit.cpp \
    db/dbdelegate.cpp \
    db/dblogin.cpp \
    db/dbmapper.cpp \
    db/dbrelationeditdialog.cpp \
    db/dbtablemodel.cpp \
    db/dbviewer.cpp \
    db/dbxlsx.cpp \
    db/executor.cpp \
    db/tablemodel.cpp \
    dialognew.cpp \
    formdoz.cpp \
    main.cpp \
    mainwindow.cpp \
    olap/axiswidget.cpp \
    olap/cubewidget.cpp \
    olap/cubic.cpp \
    olap/dialogolapflt.cpp \
    olap/formfilter.cpp \
    olap/olapmodel.cpp \
    olap/tableview.cpp \
    rels.cpp

HEADERS += \
    db/dbcombobox.h \
    db/dbdateedit.h \
    db/dbdelegate.h \
    db/dblogin.h \
    db/dbmapper.h \
    db/dbrelationeditdialog.h \
    db/dbtablemodel.h \
    db/dbviewer.h \
    db/dbxlsx.h \
    db/executor.h \
    db/tablemodel.h \
    dialognew.h \
    formdoz.h \
    mainwindow.h \
    olap/axiswidget.h \
    olap/cubewidget.h \
    olap/cubic.h \
    olap/dialogolapflt.h \
    olap/formfilter.h \
    olap/olapmodel.h \
    olap/tableview.h \
    rels.h

FORMS += \
    db/dblogin.ui \
    db/dbrelationeditdialog.ui \
    dialognew.ui \
    formdoz.ui \
    mainwindow.ui \
    olap/axiswidget.ui \
    olap/cubewidget.ui \
    olap/dialogolapflt.ui \
    olap/formfilter.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    res.qrc

DISTFILES += \
    ico.rc
