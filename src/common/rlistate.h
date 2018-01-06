#ifndef RLISTATE_H
#define RLISTATE_H

#include <QObject>
#include <inttypes.h>

class RLIState : public QObject {
  Q_OBJECT

public:
  static RLIState& instance() {
    static RLIState state;
    return state;
  }

  inline std::pair<uint, uint> radar_params() { return _radar_params; }

public slots:
  void changeRadarParams(uint peleng_length, uint peleng_count);

signals:
  void radarParamsChanged(std::pair<uint, uint> params);

private:
  RLIState(QObject* parent = 0);
  ~RLIState(void);

  // Singleton
  RLIState(RLIState const&) = delete;
  RLIState& operator= (RLIState const&) = delete;

  // Peleng length, peleng count
  std::pair<uint, uint> _radar_params;
  // Latitude, Longtitude
  std::pair<float, float> _ship_location;
};


#endif // RLISTATE_H
