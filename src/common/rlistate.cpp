#include "rlistate.h"

RLIState::RLIState(QObject* parent) : QObject(parent) {
  _radar_params = std::pair<uint, uint>(0, 0);
}

RLIState::~RLIState() {

}

void RLIState::changeRadarParams(uint peleng_length, uint peleng_count) {
  _radar_params.first = peleng_length;
  _radar_params.second = peleng_count;

  emit radarParamsChanged(_radar_params);
}
