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
    widgets \
    webkitwidgets

TARGET = WellPlater
TEMPLATE = app

SOURCES += \
    main.cpp \ 
    mainwindow.cpp \
    printwidget.cpp \
    printsetupwidget.cpp \
    connectwidget.cpp \
    baydialog.cpp \
    baywidget.cpp \
    materialswidget.cpp
FORMS += \
    mainwindow.ui \
    printwidget.ui \
    printsetupwidget.ui \
    connectwidget.ui \
    baydialog.ui \
    baywidget.ui \
    materialswidget.ui

HEADERS += \
    mainwindow.h \
    printwidget.h \
    printsetupwidget.h \
    connectwidget.h \
    baydialog.h \
    baywidget.h \
    materialswidget.h


INCLUDEPATH += FabConLib
include(FabConLib/FabConLib/include.pro)

#DEFINES += DEBUGGING
#DEFINES += DUMPING


RESOURCES += fabprint.qrc
#OTHER_FILES += fabstudio.rc
RC_FILE = fabstudio.rc

# OTHER_FILES should be referenced in include.pro.


