#pragma once

#include <QMatrix4x4>
#include <QOpenGLShaderProgram>
#include <QOpenGLWidget>

namespace qterrainview {

class OpenGLTerrain;

class OpenGLWidget : public QOpenGLWidget
{
  Q_OBJECT
public:
  OpenGLWidget(QWidget* parent);

  ~OpenGLWidget();

  OpenGLTerrain* terrain();

  bool setViewMatrix(const QMatrix4x4& viewMatrix);

  void setVerticalRange(float verticalRange) { m_verticalRange = verticalRange; }

  void setMetersPerPixel(float metersPerPixel) { m_metersPerPixel = metersPerPixel; }

  void setFieldOfView(float fieldOfView) { m_fieldOfView; }

  void setClipNear(float clipNear) { m_clipNear = clipNear; }

  void setClipFar(float clipFar) { m_clipFar = clipFar; }

signals:
  void contextInitialized();

protected:
  void initializeGL() override;

  void resizeGL(int w, int h) override;

  void paintGL() override;

  QMatrix4x4 getMVPMatrix() const;

  static QMatrix4x4 makeDefaultViewMatrix();

private:
  OpenGLTerrain* m_terrain = nullptr;

  QOpenGLShaderProgram m_terrainProgram;

  QMatrix4x4 m_viewMatrix = makeDefaultViewMatrix();

  float m_clipNear = 0.1;

  float m_clipFar = 1000;

  float m_fieldOfView = 60;

  float m_metersPerPixel = 1;

  float m_verticalRange = 1;

  int m_mvpUniform = -1;

  int m_elevationUniform = -1;

  int m_colorUniform = -1;

  int m_positionAttrib = -1;
};

} // namespace qterrainview
