SOURCES  +=   ../qextserial/qextserialenumerator.cpp\
              ../qextserial/qextserialport_win.cpp \
              ../qextserial/qextserialenumerator_win.cpp\
             ../qextserial/qextserialport.cpp


HEADERS  +=   ../qextserial/qextserialport.h \
              ../qextserial/qextserialport_p.h \
              ../qextserial/qextserialenumerator.h \
              ../qextserial/qextserialenumerator_p.h \
              ../qextserial/qextserialport_global.h



# For Windows user who doesn't have Qt's Private files
#win32:!exists($$[QT_INSTALL_HEADERS]/QtCore/private/qwineventnotifier_p.h){
        DEFINES            += QESP_NO_QT_PRIVATE
        HEADERS            +=   ../qextserial/qextwineventnotifier_p.h
        SOURCES            +=   ../qextserial/qextwineventnotifier_p.cpp
#}
win32:LIBS             += -lsetupapi -ladvapi32 -luser32

