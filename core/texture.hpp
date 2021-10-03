#pragma once

#include <QVector3D>

#include <vector>

#include <cstdint>

class Texture final
{
public:
  using Size = std::int64_t;

  Texture() = default;

  Texture(Size w, Size h);

  void clear();

  QVector3D* data() noexcept { return &m_color_buffer[0]; }

  const QVector3D* data() const noexcept { return &m_color_buffer[0]; }

  void resize(Size w, Size h);

  Size width() const noexcept { return m_width; }

  Size height() const noexcept { return m_height; }

  QVector3D& operator()(Size x, Size y) noexcept { return m_color_buffer[(y * width()) + x]; }

  const QVector3D& operator()(Size x, Size y) const noexcept
  {
    return m_color_buffer[(y * width()) + x];
  }

  QVector3D& operator[](Size index) noexcept { return m_color_buffer[index]; }

  const QVector3D& operator[](Size index) const noexcept { return m_color_buffer[index]; }

private:
  std::vector<QVector3D> m_color_buffer;

  Size m_width = 0;

  Size m_height = 0;
};
