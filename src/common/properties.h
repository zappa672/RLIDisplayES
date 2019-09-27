#ifndef PROPERTIES_H
#define PROPERTIES_H

#include <QString>

#if defined(GL_ES_VERSION_3_0) && GL_ES_VERSION_3_0
static QString SHADERS_PATH = ":/shaders/es/";
#else
static QString SHADERS_PATH = ":/shaders/core/";
#endif


// Application property names
static const char* PROPERTY_SHOW_BUTTON_PANEL   = const_cast<const char*>("PROPERTY_SHOW_BUTTON_PANEL");

static const char* PROPERTY_PELENG_SIZE         = const_cast<const char*>("PRPOPERTY_PELENG_SIZE");
static const char* PROPERTY_BEARINGS_PER_CYCLE  = const_cast<const char*>("PRPOPERTY_BEARINGS_PER_CYCLE");

static const char* PROPERTY_FRAME_DELAY         = const_cast<const char*>("PRPOPERTY_FRAME_DELAY");
static const char* PROPERTY_DATA_DELAY          = const_cast<const char*>("PRPOPERTY_DATA_DELAY");
static const char* PROPERTY_BLOCK_SIZE          = const_cast<const char*>("PRPOPERTY_BLOCK_SIZE");

static const char* PROPERTY_RLI_WIDGET_SIZE     = const_cast<const char*>("PROPERTY_RLI_WIDGET_SIZE");

#endif // PROPERTIES_H
