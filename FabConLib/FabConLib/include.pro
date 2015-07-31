SOURCES += \
    FabConLib/FabConLib/bay.cpp \
    FabConLib/FabConLib/xdflpath.cpp \
    FabConLib/FabConLib/xdflvoxel.cpp \
    FabConLib/FabConLib/material.cpp \
    FabConLib/FabConLib/xyzmotion.cpp \
    FabConLib/FabConLib/xdflhandler.cpp \
    FabConLib/FabConLib/virtualmachines.cpp \
    FabConLib/FabConLib/coreinterface.cpp \
    FabConLib/FabConLib/loadconfigthread.cpp \
    FabConLib/FabConLib/testing/util.cpp \
    FabConLib/FabConLib/testing/unittests.cpp \
    arduinoInterface/arduinoInterface/arduinointerface.cpp

HEADERS += \
    FabConLib/FabConLib/bay.h \
    FabConLib/FabConLib/xdflpath.h \
    FabConLib/FabConLib/xdflvoxel.h \
    FabConLib/FabConLib/material.h \
    FabConLib/FabConLib/xyzmotion.h \
    FabConLib/FabConLib/xdflhandler.h \
    FabConLib/FabConLib/virtualmachines.h \
    FabConLib/FabConLib/coreinterface.h \
    FabConLib/FabConLib/loadconfigthread.h \
    FabConLib/FabConLib/testing/util.h \
    FabConLib/FabConLib/testing/unittests.h \
    arduinoInterface/arduinoInterface/arduinointerface.h


OTHER_FILES += \
    media/testxdfl.xdfl \
    media/TestScript.js \
    media/testConfig.config \
    media/testxdfl-test.xdfl \
    media/testxdfl-mid.xdfl \
    media/JrKerr-Valve.config \
    media/JrKerr-Single-deposition.config \
    media/JrKerr-Duel-deposition.config

include("../../arduinoInterface/qextserialport-1.2rc/src/qextserialport.pri")
INCLUDEPATH += arduinoInterface/qextserialport-1.2rc/
INCLUDEPATH += arduinoInterface/arduinoInterface/
