#ifndef TARGETENGINE_H
#define TARGETENGINE_H

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
  explicit TargetEngine(QOpenGLContext* context, QObject* parent = 0);
  virtual ~TargetEngine();

  void draw(const QMatrix4x4& mvp_matrix, std::pair<float, float> coords, float scale);

  int getTailsTime(void);
  int getCurrentIndex();

signals:
  void targetCountChanged(int count);
  void selectedTargetUpdated(const QString& tag, const RadarTarget& trgt);

public slots:
  void onTailsTimer();
  void onTailsModeChanged(int mode, int minutes);

  void trySelect(QVector2D cursorCoords, float scale);

  void deleteTarget(QString tag);
  void updateTarget(QString tag, RadarTarget target);

private:
  void bindBuffers();
  void initBuffersTrgts(QString tag);
  int initBuffersTails();

  void initShader();
  QOpenGLTexture* initTexture(QString path);

  QMutex _trgtsMutex;
  QString _selected;
  QMap<QString, RadarTarget> _targets;

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

