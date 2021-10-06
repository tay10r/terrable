#pragma once

#include <qterrainview/QTerrainSurface>

#include "pbr_texture.h"

#include <array>
#include <memory>
#include <vector>

class QOpenGLWidget;

namespace qterrainview {

class SplatMapBuffer;

class QTerrainSurface::Self final
{
public:
  Self(QOpenGLWidget& openGLWidget,
       std::vector<std::unique_ptr<PBRTexture>>&& textures,
       const SplatMapBuffer& splatMapBuffer);

  Self(const Self&) = delete;

  ~Self();

  template<typename Visitor>
  void visit(Visitor& visitor);

private:
  QOpenGLWidget& m_openGLWidget;

  std::vector<std::unique_ptr<PBRTexture>> m_textures;

  std::vector<std::unique_ptr<QOpenGLTexture>> m_splatMaps;
};

template<typename Visitor>
void
QTerrainSurface::Self::visit(Visitor& visitor)
{
  const size_t texCount = m_textures.size();

  const size_t splatMapCount = m_splatMaps.size();

  const size_t iterations = std::min(texCount / 4, splatMapCount);

  for (size_t i = 0; i < iterations; i++) {

    QOpenGLTexture& splatMap = *m_splatMaps[i];

    std::array<PBRTexture*, 4> textures{ { m_textures[(i * 4) + 0].get(),
                                           m_textures[(i * 4) + 1].get(),
                                           m_textures[(i * 4) + 2].get(),
                                           m_textures[(i * 4) + 3].get() } };

    visitor(splatMap, textures);
  }
}

} // namespace qterrainview
