#ifndef MENUENGINE_H
#define MENUENGINE_H

#include <QDebug>
#include <QDateTime>
#include <QByteArray>

#include <QOpenGLFunctions>
#include <QOpenGLFramebufferObject>
#include <QOpenGLShaderProgram>

#include "infofonts.h"
#include "menuitem.h"

#include "../../common/rlistrings.h"
#include "../../common/rlilayout.h"
#include "../routeengine.h"


class MenuEngine : public QObject, protected QOpenGLFunctions {
  Q_OBJECT

public:
  MenuEngine(const RLIMenuLayout& layout, QOpenGLContext* context, QObject* parent = nullptr);
  virtual ~MenuEngine();

  inline QRect geometry() { return _geometry; }
  inline GLuint texture() { return _fbo->texture(); }

  inline void setFonts(InfoFonts* fonts) { _fonts = fonts; }
  void resize(const RLIMenuLayout& layout);

signals:
  void languageChanged(RLIString lang_str);
  void radarBrightnessChanged(int br);
  void simulationChanged(RLIString lang_str);

  void startRouteEdit();
  void finishRouteEdit();

  void saveRoute(int index);
  void loadRoute(int index);

  void analogZeroChanged(int val);
  void tailsModeChanged(RLIString lang_str);
  void bandModeChanged(RLIString lang_str);

public slots:
  void onStateChanged(RLIWidgetState state);
  void onLanguageChanged(RLIString lang_str);
  void onKeyPressed(QKeyEvent* event);

  void update();

  void onUp();
  void onDown();
  void onEnter();
  void onBack();

  void onAnalogZeroChanged(int val);

private:
  void initMainMenuTree();
  void initCnfgMenuTree();
  void initShader();

  enum TextAllignement { ALIGN_LEFT, ALIGN_CENTER, ALIGN_RIGHT };

  void drawBar();
  void drawSelection();

  void drawRect(const QRect& rect, const QColor& col);
  void drawText(const QByteArray& text, int line, TextAllignement align, const QColor& col);

  bool _need_update;

  QRect _geometry;

  QString _font_tag;
  QDateTime _last_action_time;

  RLIMenuItemMenu* _menu;

  RLIMenuItemMenu* _main_menu;
  RLIMenuItemMenu* _cnfg_menu;

  RLIMenuItemAction* routeEditItem;
  RLIMenuItemInt* routeLoaderItem;
  RLIMenuItemInt* routeSaverItem;

  RLIMenuItemInt* analogZeroItem;

//  RouteEngine* _routeEngine;

  int _selected_line;
  bool _selection_active;

  InfoFonts* _fonts;
  RLILang _lang;

  QOpenGLFramebufferObject* _fbo;
  QOpenGLShaderProgram* _prog;

  // -----------------------------------------------
  enum { INFO_ATTR_POSITION = 0
       , INFO_ATTR_ORDER = 1
       , INFO_ATTR_CHAR_VAL = 2
       , INFO_ATTR_COUNT = 3 } ;
  enum { INFO_UNIF_MVP = 0
       , INFO_UNIF_COLOR = 1
       , INFO_UNIF_SIZE = 2
       , INFO_UNIF_COUNT = 3 } ;

  GLuint _vbo_ids[INFO_ATTR_COUNT];
  GLuint _attr_locs[INFO_ATTR_COUNT];
  GLuint _uniform_locs[INFO_UNIF_COUNT];
};

#endif // MENUENGINE_H
