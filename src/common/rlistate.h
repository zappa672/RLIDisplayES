#ifndef RLISTATE_H
#define RLISTATE_H

#include <QObject>
#include <QVector2D>
#include <QPoint>
#include <QKeyEvent>

enum class RLIWidgetState {
  RLISTATE_DEFAULT        = 0
, RLISTATE_MAIN_MENU      = 1
, RLISTATE_CONFIG_MENU    = 2
, RLISTATE_MAGNIFIER      = 3
, RLSITATE_ROUTE_EDITION  = 4
};


struct RLIState : public QObject {
  Q_OBJECT

public:
  explicit RLIState(QObject* parent = 0);
  virtual ~RLIState();

  inline RLIWidgetState state() const { return _state; }

  inline float gain()     const { return _gain; }
  inline float water()    const { return _water; }
  inline float rain()     const { return _rain; }
  inline float apch()     const { return _apch; }
  inline float emission() const { return _emission; }

  inline float      northShift()    const { return _north_shift; }
  inline float      chartScale()    const { return _chart_scale; }
  inline QVector2D  shipPosition()  const { return _ship_position; }
  inline QPoint     centerShift()   const { return _center_shift; }

  inline float  vnP()           const { return _vn_p; }
  inline float  vnCu()          const { return _vn_cu; }
  inline float  vd()            const { return _vd; }
  inline QPoint cursorPos()     const { return _cursor_pos; }
  inline bool   showCircles()   const { return _show_circles; }
  inline bool   showParallel()  const { return _show_parallel; }

signals:
  void stateChanged(RLIWidgetState);

  void gainChanged(float);
  void waterChanged(float);
  void rainChanged(float);

public slots:
  void onKeyPressed(QKeyEvent* event, const QSet<int>& keys);

  void onShipPositionChanged(const QVector2D& pos) { _ship_position = pos; }

private:
  void save();
  void restore();

  RLIWidgetState _state { RLIWidgetState::RLISTATE_DEFAULT };

  // Value bar parameters
  float _gain      { 0.f };
  float _water     { 0.f };
  float _rain      { 0.f };
  float _apch      { 0.f };
  float _emission  { 0.f };

  // Chart parameters
  float     _north_shift   { 0.f };
  float     _chart_scale   { 100.f };
  QVector2D _ship_position { 0.0, 0.0 };
  QPoint    _center_shift  { 0, 0 };

  // Controls parameters
  float   _vn_p          { 0.f };
  float   _vn_cu         { 0.f };
  float   _vd            { 0.f };
  QPoint  _cursor_pos    { 0, 0 };
  bool    _show_circles  { false };
  bool    _show_parallel { false };
};

#endif // RLISTATE_H
