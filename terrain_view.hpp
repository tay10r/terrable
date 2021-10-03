#pragma once

#include <QMatrix4x4>
#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QOpenGLWidget>

class Terrain;

class TerrainView : public QOpenGLWidget
{
  Q_OBJECT
public:
  TerrainView(QWidget* parent);

  void loadTerrain(Terrain&);

signals:
  void contextInitialized();

protected:
  void initializeGL();

  void paintGL();

  void resizeGL(int, int);

  QMatrix4x4 makeMVPMatrix() const;

private:
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
