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

uniform vec2 pixel_size = vec2(1.0 / 64.0, 1.0 / 64.0);

uniform sampler2D elevation;

out vec2 tex_coords;

out vec3 surface_normal;

vec2
uv_to_ndc(vec2 uv)
{
  return vec2((uv.x * 2.0) - 1.0, (uv.y * 2.0) - 1.0);
}

vec3
compute_surface_normal()
{
  vec2 uv0 = position;
  vec2 uv1 = position + vec2(pixel_size.x, 0.0);
  vec2 uv2 = position + vec2(0.0, pixel_size.y);

  float y0 = texture(elevation, uv0).r;
  float y1 = texture(elevation, uv1).r;
  float y2 = texture(elevation, uv2).r;

  vec2 ndc0 = uv_to_ndc(uv0);
  vec2 ndc1 = uv_to_ndc(uv1);
  vec2 ndc2 = uv_to_ndc(uv2);

  vec3 p0 = vec3(ndc0.x, y0, ndc0.y);
  vec3 p1 = vec3(ndc1.x, y1, ndc1.y);
  vec3 p2 = vec3(ndc2.x, y2, ndc2.y);

  vec3 e0 = p1 - p0;
  vec3 e1 = p2 - p0;

  return normalize(cross(e0, e1));
}

void
main()
{
  tex_coords = position;

  surface_normal = compute_surface_normal();

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

in vec3 surface_normal;

out vec4 frag_color;

uniform vec3 light_direction = vec3(-1.0, -1.0, 0.0);

uniform sampler2D color;

uniform sampler2D elevation;

void
main()
{
  if (tex_coords.x == 0.3) {
    frag_color = texture(color, tex_coords);
  } else if (tex_coords.y == 0.3) {
    float k = texture(elevation, tex_coords).r;
    frag_color = vec4(k, k, k, 1.0);
  } else {
    float c = clamp((dot(surface_normal, -normalize(light_direction)) + 1.0) * 0.5, 0.0, 1.0);

    frag_color = vec4(c, c, c, 1.0);

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

void
OpenGLWidget::setBackgroundColor(const QColor& color)
{
  context()->functions()->glClearColor(color.redF(), color.greenF(), color.blueF(), color.alphaF());
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

  m_lightDirectionUniform = m_terrainProgram.uniformLocation("light_direction");

  assert(m_lightDirectionUniform >= 0);

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

  functions->glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

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

  QVector3D lightDirection = m_terrain->lightDirection();

  functions->glUniform3f(
    m_lightDirectionUniform, lightDirection.x(), lightDirection.y(), lightDirection.z());

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
