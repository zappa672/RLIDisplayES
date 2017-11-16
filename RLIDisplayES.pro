#-------------------------------------------------
#
# Project created by QtCreator 2017-09-16T21:34:17
#
#-------------------------------------------------

QT       += core gui widgets concurrent opengl

TARGET = RLIDisplayES
TEMPLATE = app

unix:QMAKE_CXXFLAGS += -Wno-write-strings
unix:QMAKE_CXXFLAGS += -Wno-unused-variable
unix:QMAKE_CXXFLAGS += -std=gnu++11

SOURCES     += \
    src/main.cpp \
    src/mainwindow.cpp \
    src/rlicontrolwidget.cpp \
    src/rlidisplaywidget.cpp \
    src/common/rliconfig.cpp \
    src/layers/radarengine.cpp \
    src/datasources/radarscale.cpp \
    src/datasources/radardatasource.cpp \
    src/layers/radarpalette.cpp \
    src/layers/maskengine.cpp

HEADERS     += \
    src/mainwindow.h \
    src/rlicontrolwidget.h \
    src/rlidisplaywidget.h \
    src/common/rliconfig.h \
    src/layers/radarengine.h \
    src/datasources/radarscale.h \
    src/datasources/radardatasource.h \
    src/layers/radarpalette.h \
    src/layers/maskengine.h \
    src/common/properties.h

FORMS       += \
    forms/mainwindow.ui \
    forms/rlicontrolwidget.ui

RESOURCES   += \
    res/icons.qrc \
    res/shaders.qrc \
    res/config.qrc

OTHER_FILES +=

DISTFILES +=
