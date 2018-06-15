#ifndef MENUENGINE_H
#define MENUENGINE_H

#include <QDebug>
#include <QDateTime>
#include <QByteArray>
#include <QTextEncoder>
#include <QTextDecoder>

#include <QOpenGLFunctions>
#include <QOpenGLFramebufferObject>
#include <QOpenGLShaderProgram>

#include "infofonts.h"
#include "../../common/rlistrings.h"
#include "../../common/rliconfig.h"
#include "../routeengine.h"


class RLIMenuItem : public QObject {
  Q_OBJECT
public:
  RLIMenuItem(char** name, QObject* parent = 0);

  virtual QByteArray name(int lang_id) { return _name[lang_id]; }
  virtual QByteArray value(int lang_id) { Q_UNUSED(lang_id); return QByteArray(); }
  virtual int setValue(QByteArray val) {Q_UNUSED(val); return -1; }

  inline bool enabled() { return _enabled; }
  inline void setEnabled(bool val) { _enabled = val; }

  inline bool locked() { return _locked; }
  inline void setLocked(bool val) { _locked = val; }

  virtual void up() { }
  virtual void down() { }
  virtual void action() { }

  enum RLI_MENU_ITEM_TYPE { MENU, LIST, INT, FLOAT, ACTION};

  virtual RLI_MENU_ITEM_TYPE type() { return _type; }

protected:
  RLI_MENU_ITEM_TYPE _type;

  QTextEncoder* _enc;
  QTextDecoder* _dec;

  bool _enabled;
  bool _locked;

private:
  QByteArray _name[RLI_LANG_COUNT];
};


class RLIMenuItemAction : public RLIMenuItem {
  Q_OBJECT
public:
  RLIMenuItemAction(char** name, QObject* parent = 0);
  ~RLIMenuItemAction() {}

  void action();

signals:
  void triggered();

private:
};


class RLIMenuItemMenu : public RLIMenuItem {
public:
  RLIMenuItemMenu(char** name, RLIMenuItemMenu* parent);
  ~RLIMenuItemMenu();

  inline QByteArray value(int lang_id) { Q_UNUSED(lang_id); return QByteArray(); }

  inline RLIMenuItemMenu* parent()      { return _parent; }
  inline RLIMenuItem*     item(int i)   { return _items[i]; }
  inline int              item_count()  { return _items.size(); }
  inline void add_item(RLIMenuItem* i)  { _items.push_back(i); }

private:
  RLIMenuItemMenu* _parent;
  QVector<RLIMenuItem*> _items;
};


class RLIMenuItemList : public RLIMenuItem {
  Q_OBJECT
public:
  RLIMenuItemList(char** name, int def_ind, QObject* parent = 0);
  ~RLIMenuItemList() {}

  inline QByteArray value(int lang_id) { return _variants[lang_id][_index]; }
  void addVariant(char** values);

  void up();
  void down();

signals:
  void valueChanged(const QByteArray);

private:
  int _index;
  QVector<QByteArray> _variants[RLI_LANG_COUNT];
};


class RLIMenuItemInt : public RLIMenuItem {
    Q_OBJECT
public:
  RLIMenuItemInt(char** name, int min, int max, int def, QObject* parent = 0);
  ~RLIMenuItemInt() {}

  inline QByteArray value(int lang_id) { Q_UNUSED(lang_id); return QString::number(_value).toLatin1(); }
  inline int intValue() { return _value; }
  inline int minValue() { return _min; }
  inline int maxValue() { return _max; }

  void up();
  void down();

public slots:
  int setValue(int val);
  int setValue(QByteArray val);

signals:
  void valueChanged(int);

private:
  void adjustDelta();

  QDateTime _change_start_time;
  QDateTime _last_change_time;

  int _delta;

  int _value;
  int _min, _max;
};


class RLIMenuItemFloat : public RLIMenuItem {
public:
  RLIMenuItemFloat(char** name, float min, float max, float def);
  ~RLIMenuItemFloat() { }

  inline QByteArray value(int lang_id) { Q_UNUSED(lang_id); return QString::number(_value).left(5).toLatin1(); }
  inline float fltValue() { return _value; }
  inline float minValue() { return _min; }
  inline float maxValue() { return _max; }

  inline void up() { if (_value + _step < _max) _value += _step; }
  inline void down() { if (_value - _step > _min) _value -= _step; }

private:
  float _value;
  float _step;
  float _min, _max;
};




class MenuEngine : public QObject, protected QOpenGLFunctions {
  Q_OBJECT

public:
  enum MenuState { DISABLED, MAIN, CONFIG };

  explicit MenuEngine(const RLIPanelInfo& params, QOpenGLContext* context, QObject* parent = 0);
  virtual ~MenuEngine();

  inline QPoint position() { return _position; }
  inline QSize size() { return _size; }
  inline GLuint texture() { return _fbo->texture(); }

  //inline void setRouteEngine(RouteEngine* e) { _routeEngine = e; }

  inline void setFonts(InfoFonts* fonts) { _fonts = fonts; }

  void resize(const QSize& sz, const QPoint &pos, const QString& font);

  inline MenuState state() { return _state; }
  inline bool visible() { return _state != DISABLED; }
  inline QByteArray toQByteArray(const char* str) { return _enc->fromUnicode(_dec->toUnicode(str)); }

signals:
  void languageChanged(const QByteArray& lang);
  void radarBrightnessChanged(int br);
  void simulationChanged(const QByteArray& sim);

  void startRouteEdit();
  void finishRouteEdit();

  void saveRoute(int index);
  void loadRoute(int index);

  void analogZeroChanged(int val);
  void tailsModeChanged(const QByteArray& val);
  void bandModeChanged(const QByteArray& val);

public slots:
  void setState(MenuState state);
  void onLanguageChanged(const QByteArray& lang);

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

  MenuState _state;

  QPoint _position;
  QSize _size;

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
  QTextEncoder* _enc;
  QTextDecoder* _dec;
  QTextDecoder* _dec1;

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
