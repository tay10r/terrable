#pragma once

#include <qterrainview/QTerrainSurfaceBuilder>

#include <QVector4D>

#include <memory>
#include <vector>

#include "pbr_texture.h"
#include "pbr_texture_buffer.h"
#include "splat_map_buffer.h"

class QOpenGLWidget;

namespace qterrainview {

class QTerrainSurface;

class QTerrainSurfaceBuilder::Self final
{
public:
  using Error = QTerrainSurfaceBuilder::Error;

  Self(QOpenGLWidget& openGLWidget)
    : m_openGLWidget(openGLWidget)
  {}

  ~Self();

  Error beginPBRTexture();

  Error loadAlbedo(const QVector3D* rgb, int w, int h);

  Error loadRoughness(const float* roughness, int w, int h);

  Error loadNormal(const QVector3D* normal, int w, int h);

  Error loadBump(const float* bump, int w, int h);

  Error loadSplatMap(const float* splat, int w, int h);

  Error finishPBRTexture();

  std::shared_ptr<QTerrainSurface> finishSurface();

private:
  PBRTextureBuffer& getOrMakeCurrentTexture(int w, int h);

  SplatMapBuffer& getOrMakeSplatMapBuffer(int w, int h);

  void addRequiredPadding();

private:
  QOpenGLWidget& m_openGLWidget;

  std::unique_ptr<SplatMapBuffer> m_splatMapBuffer;

  std::unique_ptr<PBRTextureBuffer> m_currentTexture;

  std::vector<std::unique_ptr<PBRTexture>> m_textures;

  std::vector<std::vector<float>> m_splatMaps;
};

} // namespace qterrainview
