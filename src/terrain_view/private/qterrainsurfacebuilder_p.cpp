#include "qterrainsurfacebuilder_p.h"

#include "qterrainsurface_p.h"

#include <QOpenGLWidget>
#include <QVector3D>

namespace qterrainview {

QTerrainSurfaceBuilder::Self::~Self()
{
  m_openGLWidget.makeCurrent();

  m_textures.clear();

  m_openGLWidget.doneCurrent();
}

auto
QTerrainSurfaceBuilder::Self::beginPBRTexture() -> Error
{
  if (m_currentTexture)
    return CallOutOfOrder;

  return NoError;
}

auto
QTerrainSurfaceBuilder::Self::loadAlbedo(const QVector3D* rgb, int w, int h) -> Error
{
  PBRTextureBuffer& currentTexture = getOrMakeCurrentTexture(w, h);

  if (!currentTexture.hasSize(w, h))
    return QTerrainSurfaceBuilder::SizeDoesNotMatch;

  currentTexture.loadAlbedo(rgb);

  return QTerrainSurfaceBuilder::NoError;
}

auto
QTerrainSurfaceBuilder::Self::loadRoughness(const float* roughness, int w, int h) -> Error
{
  PBRTextureBuffer& currentTexture = getOrMakeCurrentTexture(w, h);

  if (!currentTexture.hasSize(w, h))
    return QTerrainSurfaceBuilder::SizeDoesNotMatch;

  currentTexture.loadRoughness(roughness);

  return QTerrainSurfaceBuilder::NoError;
}

auto
QTerrainSurfaceBuilder::Self::loadNormal(const QVector3D* normal, int w, int h) -> Error
{
  PBRTextureBuffer& currentTexture = getOrMakeCurrentTexture(w, h);

  if (!currentTexture.hasSize(w, h))
    return QTerrainSurfaceBuilder::SizeDoesNotMatch;

  currentTexture.loadNormal(normal);

  return QTerrainSurfaceBuilder::NoError;
}

auto
QTerrainSurfaceBuilder::Self::loadBump(const float* bump, int w, int h) -> Error
{
  PBRTextureBuffer& currentTexture = getOrMakeCurrentTexture(w, h);

  if (!currentTexture.hasSize(w, h))
    return QTerrainSurfaceBuilder::SizeDoesNotMatch;

  currentTexture.loadBump(bump);

  return QTerrainSurfaceBuilder::NoError;
}

auto
QTerrainSurfaceBuilder::Self::loadSplatMap(const float* splatMap, int w, int h) -> Error
{
  SplatMapBuffer& splatMapBuffer = getOrMakeSplatMapBuffer(w, h);

  if (!splatMapBuffer.hasSize(w, h))
    return QTerrainSurfaceBuilder::SizeDoesNotMatch;

  splatMapBuffer.addSplatMap(splatMap, w, h);

  return QTerrainSurfaceBuilder::NoError;
}

auto
QTerrainSurfaceBuilder::Self::finishPBRTexture() -> Error
{
  if (!m_currentTexture)
    return CallOutOfOrder;

  m_textures.emplace_back(new PBRTexture());

  m_openGLWidget.makeCurrent();

  Error error = NoError;

  if (!m_textures.back()->init(*m_currentTexture))
    error = OpenGLError;

  m_currentTexture.reset();

  m_openGLWidget.doneCurrent();

  return error;
}

std::shared_ptr<QTerrainSurface>
QTerrainSurfaceBuilder::Self::finishSurface()
{
  if (m_currentTexture)
    return nullptr;

  if (!m_splatMapBuffer) {

    m_splatMapBuffer.reset(new SplatMapBuffer(1, 1));

    float one = 1.0f;

    m_splatMapBuffer->addSplatMap(&one, 1, 1);
  }

  addRequiredPadding();

  std::shared_ptr<QTerrainSurface> surface(new QTerrainSurface(
    new QTerrainSurface::Self(m_openGLWidget, std::move(m_textures), *m_splatMapBuffer)));

  m_splatMapBuffer.reset();

  return surface;
}

void
QTerrainSurfaceBuilder::Self::addRequiredPadding()
{
  m_splatMapBuffer->addRequiredPadding();

  while ((m_textures.size() % 4) != 0) {

    beginPBRTexture();

    // This RGB value is just so that the builder knows what size the padded
    // texture is suppose to be.

    QVector3D rgb(0, 0, 0);

    loadAlbedo(&rgb, 1, 1);

    finishPBRTexture();
  }
}

PBRTextureBuffer&
QTerrainSurfaceBuilder::Self::getOrMakeCurrentTexture(int w, int h)
{
  if (!m_currentTexture)
    m_currentTexture.reset(new PBRTextureBuffer(w, h));

  return *m_currentTexture;
}

SplatMapBuffer&
QTerrainSurfaceBuilder::Self::getOrMakeSplatMapBuffer(int w, int h)
{
  if (!m_splatMapBuffer)
    m_splatMapBuffer.reset(new SplatMapBuffer(w, h));

  return *m_splatMapBuffer;
}

} // namespace qterrainview
