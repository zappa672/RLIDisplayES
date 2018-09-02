#ifndef PROPERTIES_H
#define PROPERTIES_H

#include <QOpenGLFunctions>
#include <QObject>
#include <QString>

#if defined(GL_ES_VERSION_3_0) && GL_ES_VERSION_3_0
static QString SHADERS_PATH = ":/shaders/es/";
#else
static QString SHADERS_PATH = ":/shaders/core/";
#endif


// Application property names
static char* PROPERTY_SHOW_BUTTON_PANEL = "PROPERTY_SHOW_BUTTON_PANEL";

static char* PROPERTY_PELENG_SIZE = "PRPOPERTY_PELENG_SIZE";
static char* PROPERTY_BEARINGS_PER_CYCLE = "PRPOPERTY_BEARINGS_PER_CYCLE";

static char* PROPERTY_FRAME_DELAY = "PRPOPERTY_FRAME_DELAY";
static char* PROPERTY_DATA_DELAY = "PRPOPERTY_DATA_DELAY";
static char* PROPERTY_BLOCK_SIZE = "PRPOPERTY_BLOCK_SIZE";

static char* PROPERTY_RLI_WIDGET_SIZE = "PROPERTY_RLI_WIDGET_SIZE";

#endif // PROPERTIES_H
