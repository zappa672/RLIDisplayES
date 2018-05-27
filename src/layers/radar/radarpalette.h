#ifndef RADARPALETTE_H
#define RADARPALETTE_H

#include <QOpenGLFunctions>
#include <QOpenGLTexture>

// Класс для расчета радарной палитры
class RadarPalette : public QObject, protected QOpenGLFunctions {
  Q_OBJECT

public:
  RadarPalette(QOpenGLContext* context, QObject* parent = nullptr);
  ~RadarPalette();

  void setRgbVar(int var);
  void setBrightness(int br);

  inline GLuint texture() { return tex->textureId(); }

private:
  // Расчёт зависимости RGBкодов цвета от амплитуды входного сигнала
  void updatePalette();

  // Параметры:
  int rgbRLI_Var;         //Текущая палитра (день/ночь)
  int brightnessRLI;      //Яркость РЛИ 0..255

  // Текущая палитра
  QOpenGLTexture* tex;
};


#endif // RADARPALETTE_H
