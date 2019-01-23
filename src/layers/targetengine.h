#ifndef TARGETENGINE_H
#define TARGETENGINE_H

#include "../common/rlistate.h"
#include "../common/rlimath.h"

#include "../datasources/targetdatasource.h"

#include <QPoint>
#include <QMutex>
#include <QList>
#include <QTimer>
#include <QVector2D>

#include <QOpenGLTexture>
#include <QOpenGLFunctions>
#include <QOpenGLFramebufferObject>
#include <QOpenGLShaderProgram>


class TargetEngine : public QObject, protected QOpenGLFunctions {
  Q_OBJECT

public:
  explicit TargetEngine(QOpenGLContext* context, QObject* parent = nullptr);
  virtual ~TargetEngine();

  void draw(const QMatrix4x4& mvp_matrix, const RLIState& state);

  inline int targetCount() const { return _targets.size(); }
  inline const QString& selectedTag() const { return _selected; }
  inline RLITarget selectedTrgt() const { return _targets.value(_selected); }

signals:
  void targetCountChanged(int count);
  void selectedTargetUpdated(const QString& tag, const RLITarget& trgt);

protected slots:
  void timerEvent(QTimerEvent* e);

public slots:
  void onTailsModeChanged(int mode, int minutes);

  void select(const GeoPos& coords, double scale);

  void deleteTarget(QString tag);
  void updateTarget(QString tag, RLITarget target);

private:
  void bindBuffers();
  void initBuffersTrgts(QString tag);
  int initBuffersTails();

  void initShader();
  QOpenGLTexture* initTexture(QString path);

  QMutex _trgtsMutex;
  QString _selected;
  QMap<QString, RLITarget> _targets;

  QTimer _tailsTimer;
  QMap<QString, QList<QVector2D> > _tails;
  int _tailsTime; // Maximum tails time in minutes

  enum {TRG_TAIL_NUM = 4};

  // Mask shader programs
  QOpenGLTexture* _asset_tex;
  QOpenGLTexture* _selection_tex;
  QOpenGLShaderProgram* _prog;

  // -----------------------------------------------
  enum { AIS_TRGT_ATTR_COORDS = 0
       , AIS_TRGT_ATTR_ORDER = 1
       , AIS_TRGT_ATTR_HEADING = 2
       , AIS_TRGT_ATTR_COURSE = 3
       , AIS_TRGT_ATTR_ROTATION = 4
       , AIS_TRGT_ATTR_SPEED = 5
       , AIS_TRGT_ATTR_COUNT = 6 } ;
  enum { AIS_TRGT_UNIF_MVP = 0
       , AIS_TRGT_UNIF_CENTER = 1
       , AIS_TRGT_UNIF_SCALE = 2
       , AIS_TRGT_UNIF_TYPE = 3
       , AIS_TRGT_UNIF_COUNT = 4 } ;

  GLuint _ind_vbo_id;
  GLuint _vbo_ids[AIS_TRGT_ATTR_COUNT];
  GLuint _attr_locs[AIS_TRGT_ATTR_COUNT];
  GLuint _unif_locs[AIS_TRGT_UNIF_COUNT];
};

#endif // TARGETENGINE_H

