#-------------------------------------------------
#
# Project created by QtCreator 2014-05-13T19:47:34
#
#-------------------------------------------------

QT       += core\
            xml

QT       += gui

TARGET = ArduinoInterface
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

include(../qdevicewatcher-master/src/libQDeviceWatcher.pri)

win32:!wince*:LIBS += -lUser32

SOURCES += main.cpp \ 
    arduinointerface.cpp

HEADERS += \
    arduinointerface.h
    

include("../qextserialport-1.2rc/src/qextserialport.pri")
INCLUDEPATH += ../qextserialport/

