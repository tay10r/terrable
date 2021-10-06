#pragma once

#ifndef QTERRAINVIEW_H
#define QTERRAINVIEW_H

#include <QWidget>

class QMatrix4x4;
class QString;
class QImage;
class QColor;
class QVector3D;

template<typename T>
class QVector;

namespace qterrainview {

void
initResources();

class QTerrainViewTexture;
class QTerrainViewSplatMap;

/// This widget supports rendering terrains from height maps and PBR textures.
///
/// To use it, be sure to wait until @ref QTerrainView::contextInitialized has
/// been emitted before calling any of the functions.
class QTerrainView : public QWidget
{
  Q_OBJECT
public:
  QTerrainView(QWidget* parent);

  QTerrainView(QTerrainView&&) = delete;

  ~QTerrainView();

  bool setBackgroundColor(const QColor&);

  bool setLightDirection(float x, float y, float z);

  bool setLightDirection(const QVector3D&);

  //===============//
  // Splat Map API //
  //===============//

  QTerrainViewSplatMap* createSplatMap();

  void destroySplatMap(QTerrainViewSplatMap* splatMap);

  bool loadSplatMap(QTerrainViewSplatMap* splatMap,
                    const float* blendVec,
                    int w,
                    int h,
                    int textureCount);

  bool loadSplatMapFromFile(QTerrainViewSplatMap* splatMap, const QString& path);

  //=============//
  // Texture API //
  //=============//

  enum TextureKind
  {
    Albedo,
    Bump,
    Normal,
    Roughness
  };

  QTerrainViewTexture* createTexture();

  void destroyTexture(QTerrainViewTexture*);

  bool loadTexture(QTerrainViewTexture* texture, TextureKind kind, const float* rgba, int w, int h);

  bool setPixelsPerTexture(float xPixelsPerTex, float yPixelsPerTex);

  //=============//
  // Terrain API //
  //=============//

  bool resizeTerrain(int w, int h);

  bool loadElevation(const float* elevation, int w, int h);

  bool loadElevation(const QImage& image);

  bool loadElevationFromFile(const QString& path, bool resizeToFile);

  void setVerticalRange(float verticalRange);

  void setMetersPerPixel(float metersPerPixel);

  bool setViewMatrix(const QMatrix4x4&);

  static QVector<float> toElevation(const QImage&);

signals:
  void contextInitialized();

private:
  class Self;

  Self* m_self;
};

} // namespace qterrainview

#endif // QTERRAINVIEW_H
