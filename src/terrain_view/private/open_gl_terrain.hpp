#pragma once

#include <QObject>
#include <QOpenGLBuffer>
#include <QOpenGLTexture>
#include <QVector3D>

namespace qterrainview {

class OpenGLTerrain final : public QObject
{
  Q_OBJECT
public:
  using Size = signed long int;

  OpenGLTerrain(QObject* parent);

  bool init();

  void destroy();

  bool resize(Size w, Size h);

  bool setColor(const float* data, Size w, Size h);

  bool setElevation(const float* data, Size w, Size h);

  void setLightDirection(float x, float y, float z) { m_lightDirection = QVector3D(x, y, z); }

  QVector3D lightDirection() const { return m_lightDirection; }

  QOpenGLTexture* color() { return &m_color; }

  QOpenGLTexture* elevation() { return &m_elevation; }

  QOpenGLBuffer* vertexBuffer() { return &m_vertexBuffer; }

private:
  QOpenGLTexture m_color{ QOpenGLTexture::Target2D };

  QOpenGLTexture m_elevation{ QOpenGLTexture::Target2D };

  QOpenGLBuffer m_vertexBuffer{ QOpenGLBuffer::VertexBuffer };

  QVector3D m_lightDirection{ -1, -1, 0 };
};

} // namespace qterrainview
