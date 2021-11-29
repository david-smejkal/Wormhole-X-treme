# -------------------------------------------------
# Project created by QtCreator 2009-03-30T02:23:01
# -------------------------------------------------
QT += opengl
TARGET = Wormhole
TEMPLATE = app
SOURCES += main.cpp \
    cmainwindow.cpp \
    cglwidget.cpp \
    cufo.cpp \
    cglobject.cpp \
    cwormhole.cpp \
    cobj2ogl.cpp \
    cdsettings.cpp \

HEADERS += cmainwindow.h \
    cglwidget.h \
    cufo.h \
    cglobject.h \
    cwormhole.h \
    cobj2ogl.h \
    cdsettings.h \
    myinclude.h \
    vec3.h
FORMS += settings.ui
RC_FILE = wormhole.rc
