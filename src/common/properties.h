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



class RLIState : public QObject {
  Q_OBJECT

public:
  static RLIState& instance() {
    static RLIState state(0);
    return state;
  }

  std::pair<float, float> shipPosition() { return _ship_position; }
  float chartScale() { return _chart_scale; }

public slots:
  void onShipPositionChanged(std::pair<float, float> position);
  void onChartScaleChanged(float scale);

private:
  RLIState(QObject* parent = 0);
  ~RLIState(void);

  // Singleton
  RLIState(RLIState const&) = delete;
  RLIState& operator= (RLIState const&) = delete;

  float _chart_scale;
  float _north_shift;
  float _ship_direction;
  std::pair<float, float> _ship_position;
};


#endif // PROPERTIES_H
