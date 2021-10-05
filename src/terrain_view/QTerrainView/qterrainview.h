#pragma once

#ifndef QTERRAINVIEW_H
#define QTERRAINVIEW_H

#include <QWidget>

class QMatrix4x4;
class QVector2D;
class QString;
class QImage;

template<typename T>
class QVector;

class QTerrainViewTexture;
class QTerrainViewSplatMap;

class QTerrainView : public QWidget
{
  Q_OBJECT
public:
  using Size = signed long int;

  enum TextureKind
  {
    Albedo
  };

  QTerrainView(QWidget* parent);

  QTerrainView(QTerrainView&&) = delete;

  ~QTerrainView();

  QTerrainViewSplatMap* createSplatMap();

  void destroySplatMap(QTerrainViewSplatMap* splatMap);

  bool loadSplatMap(QTerrainViewSplatMap* splatMap,
                    const float* blendVec,
                    int w,
                    int h,
                    int textureCount);

  bool loadSplatMapFromFile(QTerrainViewSplatMap* splatMap, const QString& path);

  bool resizeTerrain(Size w, Size h);

  bool loadElevation(const float* elevation, int w, int h);

  bool loadElevation(const QImage& image);

  bool loadElevationFromFile(const QString& path, bool resizeToFile);

  bool setTexture(int index, TextureKind kind, const float* rgba, Size w, Size h);

  void setVerticalRange(float verticalRange);

  void setMetersPerPixel(float metersPerPixel);

  bool setPixelsPerTexture(float xPixelsPerTex, float yPixelsPerTex);

  bool setViewMatrix(const QMatrix4x4&);

  static QVector<float> toElevation(const QImage&);

signals:
  void contextInitialized();

private:
  class Self;

  Self* m_self;
};

#endif // QTERRAINVIEW_H
