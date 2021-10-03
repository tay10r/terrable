#pragma once

#include <QMatrix4x4>
#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QOpenGLWidget>

class QMouseEvent;
class QWheelEvent;

class Terrain;
class Camera;

class TerrainView : public QOpenGLWidget
{
  Q_OBJECT
public:
  TerrainView(QWidget* parent, Camera& camera);

  void loadTerrain(Terrain&);

signals:
  void contextInitialized();

protected:
  void mouseMoveEvent(QMouseEvent*) override;

  void wheelEvent(QWheelEvent*) override;

  void initializeGL() override;

  void paintGL() override;

  void resizeGL(int, int) override;

  QMatrix4x4 makeMVPMatrix() const;

private:
  Camera& m_camera;

  bool m_context_initialized = false;

  int m_terrain_vertex_count = 0;

  int m_mvp_location = -1;

  int m_position_location = -1;

  int m_texture_coordinates_location = -1;

  float m_aspect_ratio = 1;

  QOpenGLShaderProgram m_render_terrain_program;

  QOpenGLBuffer m_terrain_vertex_buffer;

  GLuint m_terrain_color_texture = 0;

  QMatrix4x4 m_view_matrix;
};
