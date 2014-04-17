#-------------------------------------------------
#
# Project created by QtCreator 2014-04-08T16:10:49
#
#-------------------------------------------------

CONFIG +=c++11
QT = core
QT +=widgets
RC_FILE = airboxlabwificonf.rc

greaterThan(QT_MAJOR_VERSION, 4) {
    QT       += serialport
} else {
    include($$QTSERIALPORT_PROJECT_ROOT/src/serialport/qt4support/serialport.prf)
}


ICON = iconwifi.icns
TARGET = "Airboxlab WiFi Configurator"
TEMPLATE = app

QMAKE_CFLAGS += -gdwarf-2
QMAKE_CXXFLAGS += -gdwarf-2
QMAKE_INFO_PLIST = ConfWifiAirboxlab.plist
SOURCES += main.cpp\
        mainwindow.cpp \
    portthread.cpp \
    threadsend.cpp \
    wifithread.cpp

HEADERS  += mainwindow.h \
    portthread.h \
    threadsend.h \
    wifithread.h

FORMS    += mainwindow.ui












