#pragma once

#include <QVector4D>

#include <vector>

class QVector3D;

namespace qterrainview {

class PBRTextureBuffer final
{
public:
  PBRTextureBuffer(int w, int h);

  int width() const noexcept { return m_width; }

  int height() const noexcept { return m_height; }

  bool hasSize(int w, int h) const noexcept { return (m_width == w) && (m_height == h); }

  void loadAlbedo(const QVector3D* rgb);

  void loadRoughness(const float* roughness);

  void loadNormal(const QVector3D* normal);

  void loadBump(const float* bump);

  const std::vector<QVector4D>& albedoRoughness() const noexcept { return m_albedoRoughness; }

  const std::vector<QVector4D>& normalBump() const noexcept { return m_normalBump; }

private:
  int m_width = 0;

  int m_height = 0;

  std::vector<QVector4D> m_albedoRoughness;

  std::vector<QVector4D> m_normalBump;
};

} // namespace qterrainview
