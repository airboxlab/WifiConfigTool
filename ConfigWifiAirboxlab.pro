#-------------------------------------------------
#
# Project created by QtCreator 2014-04-08T16:10:49
#
#-------------------------------------------------

CONFIG +=c++11
QT = core
QT +=widgets
RC_FILE = airboxlabwificonf.rc
ICON = iconwifi.icns
greaterThan(QT_MAJOR_VERSION, 4) {
    QT       += serialport
} else {
    include($$QTSERIALPORT_PROJECT_ROOT/src/serialport/qt4support/serialport.prf)
}

TARGET = ConfigWifiAirboxlab
TEMPLATE = app
DEPLOYMENT.display_name=Airboxlab WiFi Configurator
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
