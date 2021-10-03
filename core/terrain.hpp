#pragma once

#include "core/texture.hpp"

#include <QVector2D>
#include <QVector3D>

#include <map>
#include <memory>
#include <string>
#include <vector>

#include <cstdint>

class QString;

class Terrain final
{
public:
  using Size = std::int64_t;

  struct Vertex final
  {
    QVector3D position{ 0, 0, 0 };

    QVector2D texture_coordinates{ 0, 0 };
  };

  void clear();

  void resize(Size w, Size h);

  Size columns() const noexcept { return m_width; }

  Size rows() const noexcept { return m_height; }

  bool openFromHeightMap(const QString& path);

  Vertex* vertexData() noexcept;

  const Vertex* vertexData() const noexcept;

  Size vertexCount() const noexcept;

  const QVector3D* currentTextureData();

  void defineTexture(const std::string& name, Texture&& texture);

  void setCurrentTexture(const std::string& name);

private:
  Texture* currentTexture();

  Texture* createElevationTexture();

private:
  std::map<std::string, Texture> m_texture_map;

  std::string m_current_texture;

  std::vector<Vertex> m_vertex_buffer;

  Size m_width = 0;

  Size m_height = 0;
};
