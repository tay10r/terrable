#include "open_gl_widget.hpp"

#include "open_gl_terrain.hpp"

#include <QMatrix4x4>
#include <QOpenGLContext>
#include <QOpenGLFunctions>
#include <QOpenGLShader>
#include <QTimer>

namespace qterrainview {

namespace {

const char* gVertShader = R"(
#version 330 core

uniform mat4 mvp = mat4(1.0);

layout(location = 0) in vec2 position;

uniform sampler2D elevation;

out vec2 tex_coords;

void
main()
{
  tex_coords = position;

  float ndc_x = (position.x * 2.0) - 1.0;
  float ndc_z = (position.y * 2.0) - 1.0;

  float ndc_y = texture(elevation, position).r;

  vec3 p = vec3(ndc_x, ndc_y, ndc_z);

  gl_Position = mvp * vec4(p, 1.0);
}
)";

const char* gFragShader = R"(
#version 330 core

in vec2 tex_coords;

out vec4 frag_color;

uniform sampler2D color;

uniform sampler2D elevation;

void
main()
{
  if (tex_coords.x == 0.3) {
    frag_color = texture(color, tex_coords);
  } else {
    float k = texture(elevation, tex_coords).r;

    frag_color = vec4(k, k, k, 1.0);

    /* frag_color = vec4(1.0, 1.0, 1.0, 1.0); */
  }
}
)";

} // namespace

OpenGLWidget::OpenGLWidget(QWidget* parent)
  : QOpenGLWidget(parent)
{}

OpenGLWidget::~OpenGLWidget()
{
  makeCurrent();

  m_terrain->destroy();

  delete m_terrain;
}

bool
OpenGLWidget::setViewMatrix(const QMatrix4x4& view)
{
  m_viewMatrix = view;

  return true;
}

OpenGLTerrain*
OpenGLWidget::terrain()
{
  return m_terrain;
}

void
OpenGLWidget::initializeGL()
{
  QOpenGLFunctions* functions = context()->functions();

  functions->glEnable(GL_DEPTH_TEST);

  m_terrainProgram.addShaderFromSourceCode(QOpenGLShader::Vertex, gVertShader);

  m_terrainProgram.addShaderFromSourceCode(QOpenGLShader::Fragment, gFragShader);

  m_terrainProgram.link();

  m_terrainProgram.bind();

  m_mvpUniform = m_terrainProgram.uniformLocation("mvp");

  assert(m_mvpUniform >= 0);

  m_positionAttrib = m_terrainProgram.attributeLocation("position");

  assert(m_positionAttrib >= 0);

  m_elevationUniform = m_terrainProgram.uniformLocation("elevation");

  assert(m_elevationUniform >= 0);

  m_colorUniform = m_terrainProgram.uniformLocation("color");

  assert(m_colorUniform >= 0);

  m_terrainProgram.release();

  m_terrain = new OpenGLTerrain(this);

  m_terrain->init();

  assert(functions->glGetError() == GL_NO_ERROR);

  QTimer::singleShot(0, this, &OpenGLWidget::contextInitialized);
}

void
OpenGLWidget::resizeGL(int w, int h)
{
  context()->functions()->glViewport(0, 0, w, h);
}

void
OpenGLWidget::paintGL()
{
  assert(m_terrain);

  QOpenGLFunctions* functions = context()->functions();

  // Setup Color Texture

  QOpenGLTexture* color = m_terrain->color();

  assert(functions->glGetError() == GL_NO_ERROR);

  functions->glActiveTexture(GL_TEXTURE0);

  assert(functions->glGetError() == GL_NO_ERROR);

  color->bind();

  assert(functions->glGetError() == GL_NO_ERROR);

  // Setup Elevation Texture

  QOpenGLTexture* elevation = m_terrain->elevation();

  functions->glActiveTexture(GL_TEXTURE1);

  elevation->bind();

  // Setup Vertex Buffer

  QOpenGLBuffer* vertexBuffer = m_terrain->vertexBuffer();

  vertexBuffer->bind();

  // Setup Program

  m_terrainProgram.bind();

  QMatrix4x4 mvp = getMVPMatrix();

  functions->glUniformMatrix4fv(m_mvpUniform, 1, GL_FALSE, mvp.constData());

  functions->glUniform1i(m_colorUniform, 0);

  functions->glUniform1i(m_elevationUniform, 1);

  const int vertexCount = vertexBuffer->size() / (sizeof(float) * 2);

  functions->glEnableVertexAttribArray(m_positionAttrib);

  functions->glVertexAttribPointer(m_positionAttrib, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0);

  functions->glDrawArrays(GL_TRIANGLES, 0, vertexCount);

  assert(functions->glGetError() == GL_NO_ERROR);

  // Cleanup

  functions->glDisableVertexAttribArray(m_positionAttrib);

  m_terrainProgram.release();

  vertexBuffer->release();

  elevation->release();

  color->release();
}

QMatrix4x4
OpenGLWidget::getMVPMatrix() const
{
  QMatrix4x4 modelMatrix;

  modelMatrix.scale(m_metersPerPixel, m_verticalRange, m_metersPerPixel);

  const float aspectRatio = float(width()) / height();

  QMatrix4x4 projectionMatrix;

  projectionMatrix.perspective(m_fieldOfView, aspectRatio, m_clipNear, m_clipFar);

  return projectionMatrix * m_viewMatrix * modelMatrix;
}

QMatrix4x4
OpenGLWidget::makeDefaultViewMatrix()
{
  QMatrix4x4 viewMatrix;

  viewMatrix.lookAt(QVector3D(2, 2, 3), QVector3D(0, 0, 0), QVector3D(0, 1, 0));

  return viewMatrix;
}

} // namespace qterrainview
