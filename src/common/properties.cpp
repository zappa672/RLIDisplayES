#include "properties.h"

#include <QDebug>

RLIState::RLIState(QObject* parent) : QObject(parent) {
  _ship_position = std::pair<float, float>(-1.f, -1.f);
  _chart_scale = 100;
}

RLIState::~RLIState() {
}

void RLIState::onShipPositionChanged(std::pair<float, float> position) {
  _ship_position = position;
}

void RLIState::onChartScaleChanged(float scale) {
  _chart_scale = scale;
}
