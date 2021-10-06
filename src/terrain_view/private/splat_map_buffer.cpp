#include "splat_map_buffer.h"

namespace qterrainview {

SplatMapBuffer::SplatMapBuffer(int w, int h)
  : m_width(w)
  , m_height(h)
{}

void
SplatMapBuffer::addRequiredPadding()
{
  if ((m_splatMaps.size() % 4) == 0)
    return;

  std::vector<float> zeroBuffer(m_width * m_height, 0.0f);

  while ((m_splatMaps.size() % 4) != 0)
    addSplatMap(zeroBuffer.data(), m_width, m_height);
}

bool
SplatMapBuffer::addSplatMap(const float* blendData, int w, int h)
{
  if (!hasSize(w, h))
    return false;

  m_splatMaps.emplace_back(std::vector<float>(blendData, blendData + (w * h)));

  return true;
}

} // namespace qterrainview
