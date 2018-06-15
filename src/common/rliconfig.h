#ifndef RLICONFIG_H
#define RLICONFIG_H

#include "rlilayout.h"

class QXmlStreamReader;

class RLIConfig {
public:
  static RLIConfig& instance() {
    static RLIConfig config("layouts.xml");
    return config;
  }

  inline const RLILayout* currentLayout() const { return _layouts[_currentSize]; }
  const QSize currentSize(void) const;
  void setCurrentSize(const QSize& screen_size);

private:
  RLIConfig(const QString& filename);
  ~RLIConfig(void);

  // Singleton
  RLIConfig(RLIConfig const&) = delete;
  RLIConfig& operator= (RLIConfig const&) = delete;

  QSize parseSize(const QString& text);
  QPoint parsePoint(const QSize& screen_sz, const QSize& sz, const QString& text);

  QMap<QString, QString> readXMLAttributes(QXmlStreamReader* xml);

  RLILayout* readLayout(const QSize& screen_sz, QXmlStreamReader* xml);
  RLIPanelTableInfo readTableInfo(QXmlStreamReader* xml);
  void parsePanelAttributes(const QSize& scr_sz, QXmlStreamReader* xml, RLIPanelInfo* panel);
  QMap<QString, RLIPanelInfo> readPanelLayouts(const QSize& scr_sz, QXmlStreamReader* xml);

  QMap<QString, RLILayout*> _layouts;
  QString _currentSize;
  QString _defaultSize;
};

#endif // RLICONFIG_H
