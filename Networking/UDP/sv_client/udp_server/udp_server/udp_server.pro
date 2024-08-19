#QT = core

CONFIG += c++17 cmdline

COMMON_DIR=../../common/
INCLUDEPATH += $$PWD/$${COMMON_DIR}
DEPENDPATH += $$PWD/$${COMMON_DIR}

#message (PWD = $$PWD)
#message(COMMON_DIR = $$COMMON_DIR)
#message(INCLUDEPATH = $$INCLUDEPATH)


SOURCES += \
    main.cpp \
    $${COMMON_DIR}program_exception.cpp \
    $${COMMON_DIR}socket_utilities.cpp \
    $${COMMON_DIR}udp_socket.cpp \
    $${COMMON_DIR}udp_socket_impl_l.cpp \
    $${COMMON_DIR}udp_socket_impl_w.cpp \
    $${COMMON_DIR}utilities.cpp

HEADERS += \
    $${COMMON_DIR}sync_print.h \
    $${COMMON_DIR}inc.h \
    $${COMMON_DIR}program_exception.h \
    $${COMMON_DIR}socket_utilities.h \
    $${COMMON_DIR}sync_print.h \
    $${COMMON_DIR}udp_def.h \
    $${COMMON_DIR}udp_socket.h \
    $${COMMON_DIR}udp_socket_impl_l.h \
    $${COMMON_DIR}udp_socket_impl_w.h \
    $${COMMON_DIR}utilities.h

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

win32:LIBS += -lWs2_32
