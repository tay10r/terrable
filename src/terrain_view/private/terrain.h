#pragma once

#include <QMatrix4x4>
#include <QObject>
#include <QOpenGLBuffer>
#include <QOpenGLTexture>
#include <QVector3D>

namespace qterrainview {

class Terrain final : public QObject
{
  Q_OBJECT
public:
  using Size = signed long int;

  Terrain(QObject* parent);

  bool init();

  void destroy();

  bool resize(Size w, Size h);

  void setVerticalRange(float verticalRange) { m_verticalRange = verticalRange; }

  void setMetersPerPixel(float metersPerPixel) { m_metersPerPixel = metersPerPixel; }

  QMatrix4x4 modelMatrix() const
  {
    QMatrix4x4 modelMatrix;
    modelMatrix.scale(m_metersPerPixel, m_verticalRange, m_metersPerPixel);
    return modelMatrix;
  }

  bool setColor(const float* data, Size w, Size h);

  bool setElevation(const float* data, Size w, Size h);

  void setLightDirection(float x, float y, float z) { m_lightDirection = QVector3D(x, y, z); }

  QVector3D lightDirection() const { return m_lightDirection; }

  QOpenGLTexture* color() { return &m_color; }

  QOpenGLTexture* elevation() { return &m_elevation; }

  QOpenGLBuffer* vertexBuffer() { return &m_vertexBuffer; }

private:
  float m_metersPerPixel = 1;

  float m_verticalRange = 1;

  QOpenGLTexture m_color{ QOpenGLTexture::Target2D };

  QOpenGLTexture m_elevation{ QOpenGLTexture::Target2D };

  QOpenGLBuffer m_vertexBuffer{ QOpenGLBuffer::VertexBuffer };

  QVector3D m_lightDirection{ -1, -1, 0 };
};

} // namespace qterrainview
