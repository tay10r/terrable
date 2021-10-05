#include <QTerrainView>

#include "elevation_converter.h"
#include "open_gl_terrain.hpp"
#include "open_gl_widget.hpp"

#include <QImage>
#include <QVBoxLayout>

#include <cassert>

using namespace qterrainview;

class QTerrainView::Self final
{
  friend QTerrainView;

  QVBoxLayout m_layout;

  OpenGLWidget m_open_gl_widget;

  Self(QTerrainView* terrain_view)
    : m_layout(terrain_view)
    , m_open_gl_widget(terrain_view)
  {
    m_layout.addWidget(&m_open_gl_widget);

    QObject::connect(&m_open_gl_widget,
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
QTerrainView::setViewMatrix(const QMatrix4x4& viewMatrix)
{
  return m_self->m_open_gl_widget.setViewMatrix(viewMatrix);
}

bool
QTerrainView::setTexture(int index, TextureKind kind, const float* rgba, Size w, Size h)
{
  if (index > 3)
    return false;

  (void)index;
  (void)kind;
  (void)rgba;
  (void)w;
  (void)h;

  return false;
}

bool
QTerrainView::resizeTerrain(Size w, Size h)
{
  OpenGLTerrain* terrain = m_self->m_open_gl_widget.terrain();

  assert(terrain != nullptr);

  if (!terrain)
    return false;

  m_self->m_open_gl_widget.makeCurrent();

  terrain->resize(w, h);

  m_self->m_open_gl_widget.doneCurrent();

  return true;
}

void
QTerrainView::setVerticalRange(float verticalRange)
{
  m_self->m_open_gl_widget.setVerticalRange(verticalRange);
}

void
QTerrainView::setMetersPerPixel(float metersPerPixel)
{
  m_self->m_open_gl_widget.setMetersPerPixel(metersPerPixel);
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
  OpenGLTerrain* terrain = m_self->m_open_gl_widget.terrain();

  assert(terrain != nullptr);

  if (!terrain)
    return false;

  m_self->m_open_gl_widget.makeCurrent();

  const bool success = terrain->setElevation(elevation, w, h);

  m_self->m_open_gl_widget.doneCurrent();

  return success;
}

QVector<float>
QTerrainView::toElevation(const QImage& image)
{
  ElevationConverter converter(image);

  return converter.convert();
}
