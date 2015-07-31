#-------------------------------------------------
#
# Project created by QtCreator 2011-10-24T13:56:22
#
#-------------------------------------------------

QT += \
    core \
    gui \
    xml \
    script \
    widgets

TARGET = WellPlater
TEMPLATE = app

SOURCES += \
    main.cpp \ 
    mainwindow.cpp
FORMS += \
    mainwindow.ui

HEADERS += \
    mainwindow.h


INCLUDEPATH += FabConLib
include(FabConLib/FabConLib/include.pro)

#DEFINES += DEBUGGING
#DEFINES += DUMPING


RESOURCES += fabprint.qrc
#OTHER_FILES += fabstudio.rc
RC_FILE = fabstudio.rc

# OTHER_FILES should be referenced in include.pro.


