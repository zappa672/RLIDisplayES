#-------------------------------------------------
#
# Project created by QtCreator 2017-09-16T21:34:17
#
#-------------------------------------------------

QT       += core gui widgets concurrent opengl

TARGET = RLIDisplayES
TEMPLATE = app

target.path = /home/root/RLIDisplayES
INSTALLS += target


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
    src/common/properties.cpp \
    src/common/rliconfig.cpp \
    src/common/triangulate.cpp \
    src/common/rlimath.cpp \
    \
    src/datasources/radarscale.cpp \
    src/datasources/radardatasource.cpp \
    src/datasources/shipdatasource.cpp \
    src/datasources/targetdatasource.cpp \
    src/datasources/infocontrollers.cpp \
    \
    src/s52/chartmanager.cpp \
    src/s52/s52chart.cpp \
    src/s52/s52assets.cpp \
    src/s52/s52references.cpp \
    \
    src/layers/info/infofonts.cpp \
    src/layers/info/infoengine.cpp \
    src/layers/info/menuengine.cpp \
    src/layers/radar/radarengine.cpp \
    src/layers/radar/radarpalette.cpp \
    src/layers/chart/chartsettingsmodel.cpp \
    src/layers/chart/chartengine.cpp \
    src/layers/chart/chartlayers.cpp \
    src/layers/chart/chartshaders.cpp \
    src/layers/maskengine.cpp \        
    src/layers/routeengine.cpp \
    src/layers/targetengine.cpp \    
    src/layers/controlsengine.cpp \
    src/layers/magnifierengine.cpp \
    src/layers/info/infoblock.cpp

HEADERS     += \
    src/mainwindow.h \
    src/rlicontrolwidget.h \
    src/rlidisplaywidget.h \
    \
    src/common/properties.h \
    src/common/rliconfig.h \
    src/common/triangulate.h \
    src/common/rlimath.h \
    src/common/rlistrings.h \
    \
    src/datasources/infocontrollers.h \
    src/datasources/radarscale.h \
    src/datasources/radardatasource.h \
    src/datasources/targetdatasource.h \
    src/datasources/shipdatasource.h \
    \
    src/s52/chartmanager.h \
    src/s52/s52chart.h \
    src/s52/s52assets.h \
    src/s52/s52references.h \
    \
    src/layers/info/infofonts.h \
    src/layers/info/infoengine.h \
    src/layers/info/menuengine.h \
    src/layers/radar/radarengine.h \
    src/layers/radar/radarpalette.h \
    src/layers/chart/chartengine.h \
    src/layers/chart/chartlayers.h \
    src/layers/chart/chartshaders.h \
    src/layers/chart/chartsettingsmodel.h \
    src/layers/maskengine.h \    
    src/layers/routeengine.h \
    src/layers/targetengine.h \    
    src/layers/controlsengine.h \
    src/layers/magnifierengine.h \
    src/common/rlilayout.h \
    src/layers/info/infoblock.h

FORMS       += \
    forms/mainwindow.ui \
    forms/rlicontrolwidget.ui

RESOURCES   += \
    res/shaders.qrc \
    res/fonts.qrc \
    res/chartsymbols.qrc

OTHER_FILES += \

DISTFILES += \
    chart_disp_conf.xml \
    layouts.xml
