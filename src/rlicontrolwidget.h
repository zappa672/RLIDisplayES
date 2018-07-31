#ifndef RLICONTROLWIDGET_H
#define RLICONTROLWIDGET_H

#include <QWidget>
#include <QKeyEvent>

namespace Ui {
  class RLIControlWidget;
}

class RLIControlWidget : public QWidget
{
  Q_OBJECT

public:
  explicit RLIControlWidget(QObject* reciever, QWidget *parent = 0);
  ~RLIControlWidget();

private slots:
  void on_sldVN_sliderReleased();
  void on_sldVD_sliderReleased();

  void on_sldVN_sliderMoved(int position);
  void on_sldVD_sliderMoved(int position);

  void on_sldGain_valueChanged(int value);
  void on_sldWater_valueChanged(int value);
  void on_sldRain_valueChanged(int value);

  void on_btnClose_clicked();

  void on_btnRLI1_clicked();
  void on_btnRLI2_clicked();
  void on_btnRLI3_clicked();
  void on_btnRLI4_clicked();

  void on_btnTrace1_clicked();
  void on_btnTrace2_clicked();
  void on_btnTrace3_clicked();
  void on_btnTrace4_clicked();
  void on_btnTrace5_clicked();
  void on_btnTrace6_clicked();
  void on_btnMenu_clicked();
  void on_btnMagnifier_clicked();

  void on_btnMode1_clicked();
  void on_btnMode2_clicked();
  void on_btnMode3_clicked();
  void on_btnModeDec_clicked();
  void on_btnModeInc_clicked();

  void on_btnOnOff1_clicked();
  void on_btnOnOff2_clicked();
  void on_btnOnOff3_clicked();
  void on_btnOnOff4_clicked();
  void on_btnOnOff5_clicked();
  void on_btnOnOff6_clicked();


  void on_btnRLI1_released();
  void on_btnRLI2_released();
  void on_btnRLI3_released();
  void on_btnRLI4_released();

  void on_btnTrace1_released();
  void on_btnTrace2_released();
  void on_btnTrace3_released();
  void on_btnTrace4_released();
  void on_btnTrace5_released();
  void on_btnTrace6_released();
  void on_btnMenu_released();
  void on_btnMagnifier_released();

  void on_btnMode1_released();
  void on_btnMode2_released();
  void on_btnMode3_released();
  void on_btnModeDec_released();
  void on_btnModeInc_released();

  void on_btnOnOff1_released();
  void on_btnOnOff2_released();
  void on_btnOnOff3_released();
  void on_btnOnOff4_released();
  void on_btnOnOff5_released();
  void on_btnOnOff6_released();

signals:
  void vdChanged(float val);
  void vnChanged(float val);

  void gainChanged(float val);
  void waterChanged(float val);
  void rainChanged(float val);

  void closeApp();

private:
  QObject* _reciever;
  void postKeyEvent(QEvent::Type type, int key);

  int _vn_pos;
  int _vd_pos;

  Ui::RLIControlWidget *ui;
};

#endif // RLICONTROLWIDGET_H
