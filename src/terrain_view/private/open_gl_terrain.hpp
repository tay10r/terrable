#pragma once

#include <QObject>
#include <QOpenGLBuffer>
#include <QOpenGLTexture>

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

  QOpenGLTexture* color() { return &m_color; }

  QOpenGLTexture* elevation() { return &m_elevation; }

  QOpenGLBuffer* vertexBuffer() { return &m_vertexBuffer; }

private:
  QOpenGLTexture m_color{ QOpenGLTexture::Target2D };

  QOpenGLTexture m_elevation{ QOpenGLTexture::Target2D };

  QOpenGLBuffer m_vertexBuffer{ QOpenGLBuffer::VertexBuffer };
};

} // namespace qterrainview
