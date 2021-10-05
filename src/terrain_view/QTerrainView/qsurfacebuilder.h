#pragma once

#ifndef QTERRAINVIEW_QSURFACEBUILDER_H
#define QTERRAINVIEW_QSURFACEBUILDER_H

namespace qterrainview {

class QTerrainSurface;

/// Used to build the normal map, packed PBR textures and splat maps.
class QTerrainSurfaceBuilder final
{
public:
  QTerrainSurfaceBuilder();

  QTerrainSurfaceBuilder(const QTerrainSurfaceBuilder&) = delete;

  ~QTerrainSurfaceBuilder();

  void beginPBRTexture();

  void loadSplat(const float* blendData, int w, int h);

  void loadAlbedo(const float* rgba, int w, int h);

  void finishPBRTexture();

  QTerrainSurface* finishSurface();

private:
  class Self;

  Self* m_self = nullptr;
};

} // namespace qterrainview

#endif // QTERRAINVIEW_QSURFACEBUILDER_H
