#include "pbr_texture_buffer.h"

#include <QVector3D>

namespace qterrainview {

PBRTextureBuffer::PBRTextureBuffer(int w, int h)
  : m_width(w)
  , m_height(h)
  , m_albedoRoughness(w * h)
  , m_normalBump(w * h)
{
  for (int i = 0; i < (w * h); i++) {

    m_albedoRoughness[i][0] = 0.8;
    m_albedoRoughness[i][1] = 0.8;
    m_albedoRoughness[i][2] = 0.8;
    m_albedoRoughness[i][3] = 0.0;

    m_normalBump[i][0] = 0.0;
    m_normalBump[i][1] = 0.1;
    m_normalBump[i][2] = 0.0;
    m_normalBump[i][3] = 0.0;
  }
}

void
PBRTextureBuffer::loadAlbedo(const QVector3D* rgb)
{
  for (int i = 0; i < (m_width * m_height); i++) {
    m_albedoRoughness[i][0] = rgb[i][0];
    m_albedoRoughness[i][1] = rgb[i][1];
    m_albedoRoughness[i][2] = rgb[i][2];
  }
}

void
PBRTextureBuffer::loadRoughness(const float* roughness)
{
  for (int i = 0; i < (m_width * m_height); i++) {
    m_albedoRoughness[i][3] = roughness[i];
  }
}

void
PBRTextureBuffer::loadNormal(const QVector3D* normal)
{
  for (int i = 0; i < (m_width * m_height); i++) {
    m_normalBump[i][0] = normal[i][0];
    m_normalBump[i][1] = normal[i][1];
    m_normalBump[i][2] = normal[i][2];
  }
}

void
PBRTextureBuffer::loadBump(const float* bump)
{
  for (int i = 0; i < (m_width * m_height); i++) {
    m_normalBump[i][3] = bump[i];
  }
}

} // namespace qterrainview
