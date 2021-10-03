#include "texture.hpp"

Texture::Texture(Size w, Size h)
{
  resize(w, h);
}

void
Texture::clear()
{
  m_color_buffer.clear();

  m_width = 0;

  m_height = 0;
}

void
Texture::resize(Size w, Size h)
{
  clear();

  m_color_buffer.resize(w * h);

  m_width = w;

  m_height = h;
}
