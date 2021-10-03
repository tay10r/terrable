#pragma once

#include <QObject>
#include <QVector2D>
#include <QVector3D>

#include <map>
#include <memory>
#include <string>
#include <vector>

#include <cstdint>

class Terrain final : public QObject
{
  Q_OBJECT
public:
  using Size = std::int64_t;

  struct Texture final
  {
    std::vector<QVector3D> color;

    Size width = 0;

    Size height = 0;
  };

  struct Vertex final
  {
    QVector3D position{ 0, 0, 0 };

    QVector2D texture_coordinates{ 0, 0 };
  };

  void clear();

  void resize(Size w, Size h);

  Size columns() const noexcept { return m_width; }

  Size rows() const noexcept { return m_height; }

  bool openFromHeightMap(const char* path);

  const Vertex* vertexData() const noexcept;

  Size vertexCount() const noexcept;

  const QVector3D* currentTextureData();

private:
  Texture* createElevationTexture();

private:
  std::map<std::string, std::shared_ptr<Texture>> m_texture_map;

  std::shared_ptr<Texture> m_current_texture;

  std::vector<Vertex> m_vertex_buffer;

  Size m_width = 0;

  Size m_height = 0;
};