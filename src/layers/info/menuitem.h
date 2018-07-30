#ifndef MENUITEM_H
#define MENUITEM_H

#include <QDebug>
#include <QDateTime>
#include <QByteArray>

#include "../../common/rlistrings.h"

class RLIMenuItem : public QObject {
  Q_OBJECT
public:
  RLIMenuItem(RLIString name, QObject* parent = 0);
  virtual ~RLIMenuItem() { }

  enum RLI_MENU_ITEM_TYPE { MENU, LIST, INT, FLOAT, ACTION};
  virtual RLI_MENU_ITEM_TYPE type() { return _type; }

  QByteArray name(RLILang lang) { return RLIStrings::instance().string(lang, _name); }
  virtual QByteArray value(RLILang lang) { Q_UNUSED(lang); return QByteArray(); }

  inline bool enabled() { return _enabled; }
  inline void setEnabled(bool val) { _enabled = val; }

  inline bool locked() { return _locked; }
  inline void setLocked(bool val) { _locked = val; }

  virtual void up() { }
  virtual void down() { }
  virtual void action() { }

protected:
  RLI_MENU_ITEM_TYPE _type;

  bool _enabled;
  bool _locked;

private:
  RLIString _name;
};


class RLIMenuItemAction : public RLIMenuItem {
  Q_OBJECT
public:
  RLIMenuItemAction(RLIString name, QObject* parent = 0);
  ~RLIMenuItemAction() {}

  void action();

signals:
  void triggered();

private:
};


class RLIMenuItemMenu : public RLIMenuItem {
public:
  RLIMenuItemMenu(RLIString name, RLIMenuItemMenu* parent);
  ~RLIMenuItemMenu();

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
  RLIMenuItemList(RLIString name, int def_ind, QObject* parent = 0);
  ~RLIMenuItemList() {}

  inline QByteArray value(RLILang lang)   { return RLIStrings::instance().string(lang, _variants[_index]); }
  inline void addVariant(RLIString value) { _variants.push_back(value); }

  void up();
  void down();

signals:
  void valueChanged(RLIString);

private:
  int _index;
  QVector<RLIString> _variants;
};


class RLIMenuItemInt : public RLIMenuItem {
    Q_OBJECT
public:
  RLIMenuItemInt(RLIString name, int min, int max, int def, QObject* parent = 0);
  ~RLIMenuItemInt() {}

  inline QByteArray value(RLILang lang) { Q_UNUSED(lang); return QString::number(_value).toLocal8Bit(); }
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
  RLIMenuItemFloat(RLIString name, float min, float max, float def);
  ~RLIMenuItemFloat() { }

  inline QByteArray value(RLILang lang) { Q_UNUSED(lang); return QString::number(_value).left(5).toLocal8Bit(); }
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



#endif // MENUITEM_H
