#include "menuitem.h"

RLIMenuItem::RLIMenuItem(RLIString name, QObject* parent) : QObject(parent) {
  _enabled = true;
  _locked = false;
  _name = name;
}



RLIMenuItemMenu::RLIMenuItemMenu(RLIString name, RLIMenuItemMenu* parent) : RLIMenuItem(name, parent) {
  _type = MENU;
  _parent = parent;
}

RLIMenuItemMenu::~RLIMenuItemMenu() {
  qDeleteAll(_items);
}



RLIMenuItemAction::RLIMenuItemAction(RLIString name, QObject* parent) : RLIMenuItem(name, parent) {
  _type = ACTION;
}

void RLIMenuItemAction::action() {
  emit triggered();
}



RLIMenuItemList::RLIMenuItemList(RLIString name, int def_ind, QObject* parent)
  : RLIMenuItem(name, parent) {
  _type = LIST;
  _index = def_ind;
}

void RLIMenuItemList::up() {
  if (_index < _variants.size() - 1) {
    _index++;
    emit valueChanged(_variants[_index]);
  }
}

void RLIMenuItemList::down() {
  if (_index > 0) {
    _index--;
    emit valueChanged(_variants[_index]);
  }
}



RLIMenuItemInt::RLIMenuItemInt(RLIString name, int min, int max, int def, QObject* parent)
  : RLIMenuItem(name, parent) {
  _type = INT;

  _min = min;
  _max = max;
  _value = def;

  _change_start_time = QDateTime::currentDateTime().addSecs(-1);
  _last_change_time = QDateTime::currentDateTime().addSecs(-1);

  _delta=1;
}

void RLIMenuItemInt::up() {
  if (_value < _max) {
    adjustDelta();

    _value += _delta;

    if (_value > _max)
      _value = _max;

    emit valueChanged(_value);
  }
}

void RLIMenuItemInt::down() {
  if (_value > _min) {
    adjustDelta();

    _value -= _delta;

    if (_value < _min)
      _value = _min;

    emit valueChanged(_value);
  }
}

int RLIMenuItemInt::setValue(int val) {
  if((val < _min) || (val > _max))
    return -2;

  _value = val;
  return 0;
}

int RLIMenuItemInt::setValue(QByteArray val) {
  int  res;
  bool ok = false;

  res = val.toInt(&ok);
  if(!ok)
      return -1;
  if((res < _min) || (res > _max))
      return -2;
  _value = res;

  return 0;
}

void RLIMenuItemInt::adjustDelta() {
  QDateTime currentTime = QDateTime::currentDateTime();

  if (currentTime > _last_change_time.addSecs(1)) {
    _last_change_time = currentTime;
    _change_start_time = currentTime;
    _delta = 1;
  } else {
    if (_change_start_time.msecsTo(currentTime) > 5000) {
      _delta = 100;
    } else if (_change_start_time.msecsTo(currentTime) > 3000) {
      _delta = 10;
    } else {
      _delta = 1;
    }

    _last_change_time = currentTime;
  }
}




RLIMenuItemFloat::RLIMenuItemFloat(RLIString name, float min, float max, float def) : RLIMenuItem(name) {
  _type = FLOAT;
  _min = min;
  _max = max;
  _value = def;
  _step = 0.2f;
}
