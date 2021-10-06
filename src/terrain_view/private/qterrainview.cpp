#include <qterrainview/QTerrainView>

#include "elevation_converter.h"
#include "open_gl_widget.hpp"
#include "qterrainsurface_p.h"
#include "qterrainsurfacebuilder_p.h"
#include "terrain.h"

#include <QApplication>
#include <QImage>
#include <QVBoxLayout>
#include <QVector3D>

#include <cassert>

inline static void
initResources()
{
  Q_INIT_RESOURCE(shaders);
}

namespace qterrainview {

void
initResources()
{
  ::initResources();
}

class QTerrainView::Self final
{
  friend QTerrainView;

  QVBoxLayout m_layout;

  OpenGLWidget m_openGLWidget;

  bool m_contextInitialized = false;

  Self(QTerrainView* terrain_view)
    : m_layout(terrain_view)
    , m_openGLWidget(terrain_view)
  {
    m_layout.addWidget(&m_openGLWidget);

    QObject::connect(&m_openGLWidget, &OpenGLWidget::contextInitialized, [this]() {
      m_contextInitialized = true;
    });

    QObject::connect(&m_openGLWidget,
                     &OpenGLWidget::contextInitialized,
                     terrain_view,
                     &QTerrainView::contextInitialized);
  }
};

QTerrainView::QTerrainView(QWidget* parent)
  : QWidget(parent)
  , m_self(new Self(this))
{}

QTerrainView::~QTerrainView()
{
  delete m_self;
}

bool
QTerrainView::setLightDirection(float x, float y, float z)
{
  assert(m_self->m_contextInitialized);

  if (!m_self->m_contextInitialized)
    return false;

  m_self->m_openGLWidget.makeCurrent();

  Terrain* terrain = m_self->m_openGLWidget.terrain();

  terrain->setLightDirection(x, y, z);

  m_self->m_openGLWidget.doneCurrent();

  return true;
}

bool
QTerrainView::setLightDirection(const QVector3D& dir)
{
  return setLightDirection(dir.x(), dir.y(), dir.z());
}

bool
QTerrainView::setBackgroundColor(const QColor& color)
{
  assert(m_self->m_contextInitialized);

  if (!m_self->m_contextInitialized)
    return false;

  m_self->m_openGLWidget.makeCurrent();

  m_self->m_openGLWidget.setBackgroundColor(color);

  m_self->m_openGLWidget.doneCurrent();

  return true;
}

QTerrainSurfaceBuilder
QTerrainView::surfaceBuilder()
{
  return QTerrainSurfaceBuilder(new QTerrainSurfaceBuilder::Self(m_self->m_openGLWidget));
}

bool
QTerrainView::setSurface(const std::shared_ptr<QTerrainSurface>& surface)
{
  assert(m_self->m_contextInitialized);

  if (!m_self->m_contextInitialized)
    return false;

  m_self->m_openGLWidget.terrain()->setSurface(surface);

  return true;
}

bool
QTerrainView::setViewMatrix(const QMatrix4x4& viewMatrix)
{
  return m_self->m_openGLWidget.setViewMatrix(viewMatrix);
}

bool
QTerrainView::resizeTerrain(int w, int h)
{
  Terrain* terrain = m_self->m_openGLWidget.terrain();

  assert(terrain != nullptr);

  if (!terrain)
    return false;

  m_self->m_openGLWidget.makeCurrent();

  terrain->resize(w, h);

  m_self->m_openGLWidget.doneCurrent();

  return true;
}

void
QTerrainView::setVerticalRange(float verticalRange)
{
  m_self->m_openGLWidget.setVerticalRange(verticalRange);
}

void
QTerrainView::setMetersPerPixel(float metersPerPixel)
{
  m_self->m_openGLWidget.setMetersPerPixel(metersPerPixel);
}

bool
QTerrainView::loadElevationFromFile(const QString& path, bool resizeHeightMapToFile)
{
  QImage image;

  if (!image.load(path))
    return false;

  if (resizeHeightMapToFile) {
    if (!resizeTerrain(image.width(), image.height()))
      return false;
  }

  return loadElevation(image);
}

bool
QTerrainView::loadElevation(const QImage& image)
{
  QVector<float> elevation = toElevation(image);

  return loadElevation(elevation.data(), image.width(), image.height());
}

bool
QTerrainView::loadElevation(const float* elevation, int w, int h)
{
  Terrain* terrain = m_self->m_openGLWidget.terrain();

  assert(terrain != nullptr);

  if (!terrain)
    return false;

  m_self->m_openGLWidget.makeCurrent();

  const bool success = terrain->setElevation(elevation, w, h);

  m_self->m_openGLWidget.doneCurrent();

  return success;
}

QVector<float>
QTerrainView::toElevation(const QImage& image)
{
  ElevationConverter converter(image);

  return converter.convert();
}

} // namespace qterrainview
