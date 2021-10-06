#pragma once

#ifndef QTERRAINVIEW_H
#define QTERRAINVIEW_H

#include <QWidget>

#include <memory>

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

class QTerrainSurfaceBuilder;
class QTerrainSurface;

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

  QTerrainSurfaceBuilder surfaceBuilder();

  bool setSurface(const std::shared_ptr<QTerrainSurface>& surface);

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
