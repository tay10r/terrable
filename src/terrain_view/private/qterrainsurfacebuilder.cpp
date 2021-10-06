#include <qterrainview/QTerrainSurfaceBuilder>

#include "qterrainsurfacebuilder_p.h"

#include <QImage>
#include <QVector3D>

namespace qterrainview {

namespace {

QVector<QVector3D>
imageToXYZ(const QImage& image)
{
  const int w = image.width();
  const int h = image.height();

  QVector<QVector3D> colors(w * h);

  for (int i = 0; i < (w * h); i++) {

    const int x = i % w;
    const int y = i / w;

    const QColor c = image.pixelColor(x, y);

    colors[i][0] = c.redF();
    colors[i][1] = c.greenF();
    colors[i][2] = c.blueF();
  }

  return colors;
}

QVector<float>
imageToFloatVector(const QImage& image)
{
  const int w = image.width();
  const int h = image.height();

  QVector<float> values(w * h);

  for (int i = 0; i < (w * h); i++) {

    const int x = i % w;
    const int y = i / w;

    values[i] = image.pixelColor(x, y).lightness();
  }

  return values;
}

} // namespace

QTerrainSurfaceBuilder::QTerrainSurfaceBuilder(QTerrainSurfaceBuilder&& other)
  : m_self(other.m_self)
{
  other.m_self = nullptr;
}

QTerrainSurfaceBuilder::QTerrainSurfaceBuilder(Self* self)
  : m_self(self)
{}

QTerrainSurfaceBuilder::~QTerrainSurfaceBuilder()
{
  delete m_self;
}

auto
QTerrainSurfaceBuilder::beginPBRTexture() -> Error
{
  return m_self->beginPBRTexture();
}

QTerrainSurfaceBuilder::Error
QTerrainSurfaceBuilder::loadAlbedo(const QVector3D* rgb, int w, int h)
{
  return m_self->loadAlbedo(rgb, w, h);
}

QTerrainSurfaceBuilder::Error
QTerrainSurfaceBuilder::loadNormal(const QVector3D* normal, int w, int h)
{
  return m_self->loadNormal(normal, w, h);
}

QTerrainSurfaceBuilder::Error
QTerrainSurfaceBuilder::loadRoughness(const float* roughness, int w, int h)
{
  return m_self->loadRoughness(roughness, w, h);
}

QTerrainSurfaceBuilder::Error
QTerrainSurfaceBuilder::loadBump(const float* bump, int w, int h)
{
  return m_self->loadBump(bump, w, h);
}

QTerrainSurfaceBuilder::Error
QTerrainSurfaceBuilder::loadSplat(const float* splat, int w, int h)
{
  return m_self->loadSplatMap(splat, w, h);
}

QTerrainSurfaceBuilder::Error
QTerrainSurfaceBuilder::loadAlbedoFromFile(const QString& path)
{
  QImage image;

  if (!image.load(path))
    return FailedToOpen;

  const int w = image.width();
  const int h = image.height();

  const QVector<QVector3D> colors = imageToXYZ(image);

  return loadAlbedo(colors.constData(), w, h);
}

QTerrainSurfaceBuilder::Error
QTerrainSurfaceBuilder::loadRoughnessFromFile(const QString& path)
{
  QImage image;

  if (!image.load(path))
    return FailedToOpen;

  const int w = image.width();
  const int h = image.height();

  const QVector<float> roughness = imageToFloatVector(image);

  return loadRoughness(roughness.constData(), w, h);
}

QTerrainSurfaceBuilder::Error
QTerrainSurfaceBuilder::loadNormalFromFile(const QString& path)
{
  QImage image;

  if (!image.load(path))
    return FailedToOpen;

  const int w = image.width();
  const int h = image.height();

  const QVector<QVector3D> colors = imageToXYZ(image);

  return loadNormal(colors.constData(), w, h);
}

QTerrainSurfaceBuilder::Error
QTerrainSurfaceBuilder::loadBumpFromFile(const QString& path)
{
  QImage image;

  if (!image.load(path))
    return FailedToOpen;

  const int w = image.width();
  const int h = image.height();

  const QVector<float> bump = imageToFloatVector(image);

  return loadBump(bump.constData(), w, h);
}

QTerrainSurfaceBuilder::Error
QTerrainSurfaceBuilder::loadSplatFromFile(const QString& path)
{
  QImage image;

  if (!image.load(path))
    return FailedToOpen;

  const int w = image.width();
  const int h = image.height();

  const QVector<float> values = imageToFloatVector(image);

  return loadSplat(values.constData(), w, h);
}

auto
QTerrainSurfaceBuilder::finishPBRTexture() -> Error
{
  return m_self->finishPBRTexture();
}

std::shared_ptr<QTerrainSurface>
QTerrainSurfaceBuilder::finishSurface()
{
  return m_self->finishSurface();
}

} // namespace qterrainview
