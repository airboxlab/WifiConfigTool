#-------------------------------------------------
#
# Project created by QtCreator 2014-04-08T16:10:49
#
#-------------------------------------------------

CONFIG +=c++11
QT = core
QT +=widgets


greaterThan(QT_MAJOR_VERSION, 4) {
    QT       += serialport
} else {
    include($$QTSERIALPORT_PROJECT_ROOT/src/serialport/qt4support/serialport.prf)
}

TARGET = ConfigWifiAirboxlab
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    portthread.cpp

HEADERS  += mainwindow.h \
    portthread.h

FORMS    += mainwindow.ui
