#pragma once

#include <QMatrix4x4>
#include <QOpenGLShaderProgram>
#include <QOpenGLWidget>

#include "terrain.h"
#include "terrain_render_program.h"

class QColor;

namespace qterrainview {

class OpenGLWidget : public QOpenGLWidget
{
  Q_OBJECT
public:
  OpenGLWidget(QWidget* parent);

  ~OpenGLWidget();

  Terrain* terrain();

  void setBackgroundColor(const QColor&);

  bool setViewMatrix(const QMatrix4x4& viewMatrix);

  void setVerticalRange(float verticalRange)
  {
    assert(m_terrain);

    m_terrain->setVerticalRange(verticalRange);
  }

  void setMetersPerPixel(float metersPerPixel)
  {
    assert(m_terrain);

    m_terrain->setMetersPerPixel(metersPerPixel);
  }

  void setFieldOfView(float fieldOfView) { m_fieldOfView = fieldOfView; }

  void setClipNear(float clipNear) { m_clipNear = clipNear; }

  void setClipFar(float clipFar) { m_clipFar = clipFar; }

  QMatrix4x4 projectionMatrix() const;

signals:
  void contextInitialized();

protected:
  void initializeGL() override;

  void resizeGL(int w, int h) override;

  void paintGL() override;

  static QMatrix4x4 makeDefaultViewMatrix();

private:
  Terrain* m_terrain = nullptr;

  QMatrix4x4 m_viewMatrix = makeDefaultViewMatrix();

  float m_clipNear = 0.1;

  float m_clipFar = 1000;

  float m_fieldOfView = 60;

  TerrainRenderProgram m_terrainRenderProgram;
};

} // namespace qterrainview
