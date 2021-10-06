#pragma once

#ifndef QTERRAINVIEW_QTERRAINSURFACE_H
#define QTERRAINVIEW_QTERRAINSURFACE_H

namespace qterrainview {

class QTerrainSurfaceBuilder;
class TerrainRenderProgram;

class QTerrainSurface final
{
public:
  QTerrainSurface() = delete;

  ~QTerrainSurface();

private:
  friend QTerrainSurfaceBuilder;

  friend TerrainRenderProgram;

  class Self;

  QTerrainSurface(Self* self);

private:
  Self* m_self = nullptr;
};

} // namespace qterrainview

#endif // QTERRAINVIEW_QTERRAINSURFACE_H
