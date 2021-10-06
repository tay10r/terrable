#pragma once

#include <vector>

namespace qterrainview {

class SplatMapBuffer final
{
public:
  SplatMapBuffer(int w, int h);

  /// Will round off splat map to upper boundary of four.
  ///
  /// @note Does not handle the case that no splat map was given by the user.
  void addRequiredPadding();

  size_t size() const noexcept { return m_splatMaps.size(); }

  bool hasSize(int w, int h) const noexcept { return (w == m_width) && (h == m_height); }

  bool addSplatMap(const float* blendData, int w, int h);

  int height() const noexcept { return m_height; }

  int width() const noexcept { return m_width; }

  const std::vector<float>& operator[](int index) const { return m_splatMaps[index]; }

private:
  int m_width = 0;

  int m_height = 0;

  std::vector<std::vector<float>> m_splatMaps;
};

} // namespace qterrainview
