#include "terrain.hpp"

#include <QImage>

#include <algorithm>
#include <limits>

namespace {

class VertexGenerator final
{
public:
  using Size = Terrain::Size;

  VertexGenerator(const std::vector<float>& elevation, Size w, Size h)
    : m_elevation(elevation.data())
    , m_width(w)
    , m_height(h)
  {}

  Size rows() const { return std::max(m_height, Size(2)); }

  Size columns() const { return std::max(m_width, Size(2)); }

  Size vertexCount() const { return rows() * columns() * 6; }

  float elevationAt(Size x, Size y) const
  {
    if ((m_width <= 0) || (m_height <= 0))
      return 0;

    x = std::min(x, m_width - 1);
    y = std::min(y, m_height - 1);

    x = std::max(x, Size(0));
    y = std::max(y, Size(0));

    return m_elevation[(y * m_width) + x];
  }

  QVector3D positionAt(Size x, Size y) const
  {
    x = std::max(x, Size(0));
    y = std::max(y, Size(0));

    x = std::min(x, columns() - 1);
    y = std::min(y, rows() - 1);

    const float u = x / float(columns());
    const float v = y / float(rows());

    const float pos_x = (u * 2) - 1;
    const float pos_y = elevationAt(x, y);
    const float pos_z = (v * 2) - 1;

    return QVector3D(pos_x, pos_y, pos_z);
  }

  QVector2D textureCoordinateAt(Size x, Size y) const
  {
    x = std::max(x, Size(0));
    y = std::max(y, Size(0));

    x = std::min(x, columns() - 1);
    y = std::min(y, rows() - 1);

    const float u = (x + 0.5f) / float(columns());
    const float v = (y + 0.5f) / float(rows());

    return QVector2D(u, v);
  }

private:
  const float* m_elevation;

  Size m_width;

  Size m_height;
};

} // namespace

void
Terrain::clear()
{
  m_vertex_buffer.clear();

  m_width = 0;

  m_height = 0;

  m_texture_map.clear();

  m_current_texture.clear();
}

void
Terrain::resize(Size w, Size h)
{
  clear();

  m_vertex_buffer.resize(w * h * 6, Vertex{});

  m_width = w;

  m_height = h;

  auto genTexCoord = [w, h](Size x, Size y) -> QVector2D {
    const float u = (x + 0.5f) / w;
    const float v = (y + 0.5f) / h;
    return QVector2D(u, v);
  };

  for (Size i = 0; i < (w * h); i++) {

    const Size x = i % w;
    const Size y = i / w;

    const QVector2D p00 = genTexCoord(x + 0, y + 0);
    const QVector2D p01 = genTexCoord(x + 0, y + 1);
    const QVector2D p10 = genTexCoord(x + 1, y + 0);
    const QVector2D p11 = genTexCoord(x + 1, y + 1);

    const Size vertexOffset = i * 6;

    m_vertex_buffer[vertexOffset + 0].texture_coordinates = p00;
    m_vertex_buffer[vertexOffset + 1].texture_coordinates = p01;
    m_vertex_buffer[vertexOffset + 2].texture_coordinates = p11;

    m_vertex_buffer[vertexOffset + 3].texture_coordinates = p11;
    m_vertex_buffer[vertexOffset + 4].texture_coordinates = p10;
    m_vertex_buffer[vertexOffset + 5].texture_coordinates = p00;
  }
}

bool
Terrain::openFromHeightMap(const QString& path)
{
  QImage image;

  if (!image.load(path))
    return false;

  float min_height = std::numeric_limits<float>::infinity();

  float max_height = -std::numeric_limits<float>::infinity();

  std::vector<float> elevation(image.width() * image.height());

  for (Size i = 0; i < (image.width() * image.height()); i++) {

    const Size x = i % image.width();
    const Size y = i / image.width();

    const float height = qGray(image.pixel(x, y)) / 255.0f;

    min_height = std::min(min_height, height);
    max_height = std::max(max_height, height);

    elevation[i] = height;
  }

  VertexGenerator vertex_generator(elevation, image.width(), image.height());

  resize(vertex_generator.columns(), vertex_generator.rows());

#pragma omp parallel for

  for (Size i = 0; i < (vertex_generator.columns() * vertex_generator.rows()); i++) {

    const Size x = i % vertex_generator.columns();
    const Size y = i / vertex_generator.columns();

    const QVector3D p00 = vertex_generator.positionAt(x + 0, y + 0);
    const QVector3D p01 = vertex_generator.positionAt(x + 0, y + 1);
    const QVector3D p10 = vertex_generator.positionAt(x + 1, y + 0);
    const QVector3D p11 = vertex_generator.positionAt(x + 1, y + 1);

    const Size vertexOffset = i * 6;

    m_vertex_buffer[vertexOffset + 0].position = p00;
    m_vertex_buffer[vertexOffset + 1].position = p01;
    m_vertex_buffer[vertexOffset + 2].position = p11;

    m_vertex_buffer[vertexOffset + 3].position = p11;
    m_vertex_buffer[vertexOffset + 4].position = p10;
    m_vertex_buffer[vertexOffset + 5].position = p00;
  }

  return true;
}

void
Terrain::defineTexture(const std::string& name, Texture&& texture)
{
  m_texture_map.emplace(name, std::move(texture));
}

Terrain::Vertex*
Terrain::vertexData() noexcept
{
  return &m_vertex_buffer[0];
}

const Terrain::Vertex*
Terrain::vertexData() const noexcept
{
  return &m_vertex_buffer[0];
}

Terrain::Size
Terrain::vertexCount() const noexcept
{
  return m_vertex_buffer.size();
}

void
Terrain::setCurrentTexture(const std::string& name)
{
  m_current_texture = name;
}

const QVector3D*
Terrain::currentTextureData()
{
  Texture* current_texture = currentTexture();
  if (!current_texture)
    return createElevationTexture()->data();
  else
    return current_texture->data();
}

Texture*
Terrain::currentTexture()
{
  if (m_current_texture.empty())
    return nullptr;

  auto it = m_texture_map.find(m_current_texture);
  if (it == m_texture_map.end())
    return nullptr;

  return &it->second;
}

Texture*
Terrain::createElevationTexture()
{
  Texture texture;

  texture.resize(columns(), rows());

  float min_height = m_vertex_buffer[0].position[1];
  float max_height = m_vertex_buffer[0].position[1];

  const Size i_max = rows() * columns();

  for (Size i = 1; i < i_max; i++) {
    min_height = std::min(min_height, m_vertex_buffer[i * 6].position[1]);
    max_height = std::max(max_height, m_vertex_buffer[i * 6].position[1]);
  }

  const float scale = 1.0f / (max_height - min_height);

  for (Size i = 0; i < i_max; i++) {

    const float color = (m_vertex_buffer[i * 6].position[1] - min_height) * scale;

    texture[i] = QVector3D(color, color, color);
  }

  return &m_texture_map.emplace("Elevation", std::move(texture)).first->second;
}
