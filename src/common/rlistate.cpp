#include "rlistate.h"

RLIState::RLIState(QObject* parent) : QObject(parent) {

}

RLIState::~RLIState() {

}


void RLIState::onKeyPressed(QKeyEvent* event, const QSet<int>& keys) {
  auto mod_keys = event->modifiers();

  switch(event->key()) {
  case Qt::Key_PageUp:
    if (mod_keys & Qt::ControlModifier)
      emit gainChanged( _gain = qMin(_gain + 5.0, 255.0) );

    if (mod_keys & Qt::AltModifier)
      emit waterChanged( _water = qMin(_water + 5.0, 255.0) );

    if (mod_keys & Qt::ShiftModifier)
      emit rainChanged( _rain = qMin(_rain + 5.0, 255.0) );

    break;

  case Qt::Key_PageDown:
    if (mod_keys & Qt::ControlModifier)
      emit gainChanged( _gain = qMax(_gain - 5.0, 0.0) );

    if (mod_keys & Qt::AltModifier)
      emit waterChanged( _water = qMax(_water - 5.0, 0.0) );

    if (mod_keys & Qt::ShiftModifier)
      emit rainChanged( _rain = qMax(_rain - 5.0, 0.0) );

    break;

  // Под. имп. Помех
  case Qt::Key_S:
    break;

  // Меню
  case Qt::Key_W:
    if(keys.contains(Qt::Key_B)) {
      if (_state == RLIWidgetState::RLISTATE_CONFIG_MENU)
        _state = RLIWidgetState::RLISTATE_DEFAULT;
      else
        _state = RLIWidgetState::RLISTATE_CONFIG_MENU;
      } else {
        if (_state == RLIWidgetState::RLISTATE_MAIN_MENU)
          _state = RLIWidgetState::RLISTATE_DEFAULT;
        else
          _state = RLIWidgetState::RLISTATE_MAIN_MENU;
      }

    emit stateChanged(_state);
    break;

  // Шкала +
  case Qt::Key_Plus:
    _chart_scale *= 0.95;
    break;

  // Шкала -
  case Qt::Key_Minus:
    _chart_scale *= 1.05;
    break;

  // Вынос центра
  case Qt::Key_C:
    break;

  // Скрытое меню
  case Qt::Key_U:
    break;

  // Следы точки
  case Qt::Key_T:
    break;

  // Выбор цели
  case Qt::Key_Up:
    _vd += 1.f;
    break;

  // ЛИД / ЛОД
  case Qt::Key_Down:
    _vd = qMax(0.f, _vd - 1.f);
    break;

  case Qt::Key_Left:
    _vn_p = fmod(_vn_p - 1.f, 360);
    break;

  case Qt::Key_Right:
    _vn_p = fmod(_vn_p + 1.f, 360);
    break;

  // Захват
  case Qt::Key_Return:
  case Qt::Key_Enter:
    break;

  //Сброс
  case Qt::Key_Escape:
    break;

  // Парал. Линии
  case Qt::Key_Backslash:
    _show_parallel = !_show_parallel;
    break;

  //Электронная лупа
  case Qt::Key_L:
    if (   _state != RLIWidgetState::RLISTATE_MAIN_MENU
        && _state != RLIWidgetState::RLISTATE_CONFIG_MENU )
      _state = RLIWidgetState::RLISTATE_MAGNIFIER;
    break;

  //Обзор
  case Qt::Key_X:
    break;

  //Узкий / Шир.
  case Qt::Key_Greater:
    break;

  //Накоп. Видео
  case Qt::Key_V:
    break;

  //Сброс АС
  case Qt::Key_Q:
    break;

  //Манёвр
  case Qt::Key_M:
    break;

  //Курс / Север / Курс стаб
  case Qt::Key_H:
    break;

  //ИД / ОД
  case Qt::Key_R:
    break;

  //НКД
  case Qt::Key_D:
    break;

  //Карта (Маршрут)
  case Qt::Key_A:
    break;

  //Выбор
  case Qt::Key_G:
    break;

  //Стоп-кадр
  case Qt::Key_F:
    break;

  //Откл. Звука
  case Qt::Key_B:
    break;

  //Откл. ОК
  case Qt::Key_K:
    break;

  //Вынос ВН/ВД
  case Qt::Key_Slash:
    _show_circles = !_show_circles;
    break;
  }
}

void RLIState::save() {

}

void RLIState::restore() {

}
