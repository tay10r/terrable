#include "terrain_view.hpp"

#include "camera.hpp"
#include "terrain.hpp"

#include <QOpenGLContext>
#include <QOpenGLFunctions>
#include <QTimer>

#include <QDebug>

#include <cassert>

TerrainView::TerrainView(QWidget* parent, Camera& camera)
  : QOpenGLWidget(parent)
  , m_camera(camera)
  , m_terrain_color_texture(QOpenGLTexture::Target2D)
{
  m_view_matrix.lookAt(QVector3D(2, 2, 3), QVector3D(0, 0, 0), QVector3D(0, 1, 0));

  setFocusPolicy(Qt::StrongFocus);

  setMouseTracking(true);
}

void
TerrainView::loadTerrain(Terrain& terrain)
{
  assert(m_context_initialized);

  makeCurrent();

  QOpenGLFunctions* functions = context()->functions();

  // Setup vertex buffer

  bool success = m_terrain_vertex_buffer.bind();

  assert(success);

  static_assert(sizeof(Terrain::Vertex) == (sizeof(float) * 5),
                "Vertex has padding, but it should not.");

  functions->glBufferData(GL_ARRAY_BUFFER,
                          sizeof(Terrain::Vertex) * terrain.vertexCount(),
                          terrain.vertexData(),
                          GL_STATIC_DRAW);

  assert(functions->glGetError() == GL_NO_ERROR);

  // Setup color texture

  functions->glBindTexture(GL_TEXTURE_2D, m_terrain_color_texture);

  functions->glTexImage2D(GL_TEXTURE_2D,
                          0,
                          GL_RGB,
                          terrain.columns(),
                          terrain.rows(),
                          0,
                          GL_RGB,
                          GL_FLOAT,
                          terrain.currentTextureData());

  functions->glBindTexture(GL_TEXTURE_2D, 0);

  assert(context()->functions()->glGetError() == GL_NO_ERROR);

  doneCurrent();

  m_terrain_vertex_count = terrain.vertexCount();

  update();
}

void
TerrainView::initializeGL()
{
  QOpenGLFunctions* functions = context()->functions();

  functions->glEnable(GL_DEPTH_TEST);

  // Compile shader

  bool success = m_render_terrain_program.addShaderFromSourceFile(QOpenGLShader::Vertex,
                                                                  ":/shaders/render_terrain.vert");

  assert(success);

  success = m_render_terrain_program.addShaderFromSourceFile(QOpenGLShader::Fragment,
                                                             ":/shaders/render_terrain.frag");

  assert(success);

  success = m_render_terrain_program.link();

  assert(success);

  // Create color texture

  functions->glActiveTexture(GL_TEXTURE0);

  functions->glGenTextures(1, &m_terrain_color_texture);

  functions->glBindTexture(GL_TEXTURE_2D, m_terrain_color_texture);

  functions->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  functions->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  functions->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  functions->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  functions->glBindTexture(GL_TEXTURE_2D, 0);

  assert(functions->glGetError() == GL_NO_ERROR);

  // Create vertex buffer

  success = m_terrain_vertex_buffer.create();

  assert(success);

  m_mvp_location = m_render_terrain_program.uniformLocation("mvp");

  assert(m_mvp_location >= 0);

  m_position_location = m_render_terrain_program.attributeLocation("position");

  assert(m_position_location >= 0);

  m_texture_coordinates_location =
    m_render_terrain_program.attributeLocation("texture_coordinates");

  assert(m_texture_coordinates_location >= 0);

  m_context_initialized = true;

  QTimer::singleShot(0, this, &TerrainView::contextInitialized);
}

void
TerrainView::paintGL()
{
  QOpenGLFunctions* functions = context()->functions();

  functions->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  functions->glUseProgram(m_render_terrain_program.programId());

  functions->glBindBuffer(GL_ARRAY_BUFFER, m_terrain_vertex_buffer.bufferId());

  // Setup texture

  const int texture_location =
    functions->glGetUniformLocation(m_render_terrain_program.programId(), "terrain_color");

  functions->glBindTexture(GL_TEXTURE_2D, m_terrain_color_texture);

  functions->glActiveTexture(GL_TEXTURE0);

  functions->glUniform1i(texture_location, 0);

  // Set MVP

  const QMatrix4x4 mvp_matrix = makeMVPMatrix();

  functions->glUniformMatrix4fv(m_mvp_location, 1, GL_FALSE, mvp_matrix.constData());

  // Set position attribute

  functions->glEnableVertexAttribArray(m_position_location);

  functions->glEnableVertexAttribArray(m_texture_coordinates_location);

  functions->glVertexAttribPointer(
    m_position_location, 3, GL_FLOAT, GL_FALSE, sizeof(Terrain::Vertex), 0);

  functions->glVertexAttribPointer(m_texture_coordinates_location,
                                   2,
                                   GL_FLOAT,
                                   GL_FALSE,
                                   sizeof(Terrain::Vertex),
                                   (void*)(sizeof(float) * 3));

  functions->glDrawArrays(GL_TRIANGLES, 0, m_terrain_vertex_count);

  assert(functions->glGetError() == GL_NO_ERROR);

  functions->glBindTexture(GL_TEXTURE_2D, 0);

  functions->glDisableVertexAttribArray(m_position_location);

  functions->glDisableVertexAttribArray(m_texture_coordinates_location);

  m_terrain_vertex_buffer.release();

  m_render_terrain_program.release();
}

void
TerrainView::resizeGL(int w, int h)
{
  context()->functions()->glViewport(0, 0, w, h);

  m_aspect_ratio = float(w) / h;
}

void
TerrainView::mouseMoveEvent(QMouseEvent* event)
{
  m_camera.mouseMoveEvent(event);

  QOpenGLWidget::mouseMoveEvent(event);
}

void
TerrainView::wheelEvent(QWheelEvent* event)
{
  m_camera.wheelEvent(event);

  QOpenGLWidget::wheelEvent(event);
}

QMatrix4x4
TerrainView::makeMVPMatrix() const
{
  QMatrix4x4 projection_matrix;

  projection_matrix.perspective(60, m_aspect_ratio, 0.01, 100.0);

  QMatrix4x4 model_matrix;

  return projection_matrix * m_camera.viewMatrix() * model_matrix;
}
