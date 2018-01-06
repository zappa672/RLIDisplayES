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

# include gdal
win32:QMAKE_LIBDIR += C:/GDAL/lib
win32:INCLUDEPATH += C:/GDAL/include
win32:LIBS += -lgdal_i -lgeos_i

unix:LIBS += -lgdal  -lrt


SOURCES     += \
    src/main.cpp \
    src/mainwindow.cpp \
    src/rlicontrolwidget.cpp \
    src/rlidisplaywidget.cpp \
    \
    src/common/rliconfig.cpp \
    src/common/rlistate.cpp \
    src/common/triangulate.cpp \
    src/common/rlimath.cpp \
    \
    src/datasources/radarscale.cpp \
    src/datasources/radardatasource.cpp \
    src/datasources/shipdatasource.cpp \
    \
    src/s52/chartmanager.cpp \
    src/s52/s52chart.cpp \
    src/s52/s52assets.cpp \
    src/s52/s52references.cpp \
    \
    src/layers/radarengine.cpp \
    src/layers/radarpalette.cpp \
    src/layers/chartengine.cpp \
    src/layers/chartlayers.cpp \
    src/layers/chartshaders.cpp \
    src/layers/maskengine.cpp

HEADERS     += \
    src/mainwindow.h \
    src/rlicontrolwidget.h \
    src/rlidisplaywidget.h \
    \
    src/common/properties.h \
    src/common/rliconfig.h \
    src/common/rlistate.h \
    src/common/triangulate.h \
    src/common/rlimath.h \
    src/common/rlistrings.h \
    \
    src/datasources/radarscale.h \
    src/datasources/radardatasource.h \
    src/datasources/shipdatasource.h \
    \
    src/s52/chartmanager.h \
    src/s52/s52chart.h \
    src/s52/s52assets.h \
    src/s52/s52references.h \
    \
    src/layers/radarengine.h \
    src/layers/radarpalette.h \
    src/layers/chartengine.h \
    src/layers/chartlayers.h \
    src/layers/chartshaders.h \
    src/layers/maskengine.h

FORMS       += \
    forms/mainwindow.ui \
    forms/rlicontrolwidget.ui

RESOURCES   += \
    res/icons.qrc \
    res/shaders.qrc \
    res/config.qrc

OTHER_FILES +=

DISTFILES +=
