#include "properties.h"

RLIState::RLIState(QObject* parent) : QObject(parent) {
  _ship_position = std::pair<float, float>(-1.f, -1.f);
}

RLIState::~RLIState() {
}

void RLIState::onShipPositionChanged(std::pair<float, float> position) {
  _ship_position = position;
}
