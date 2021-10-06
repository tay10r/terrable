#include "terrain_render_program.h"

#include "terrain.h"

#include <QOpenGLContext>
#include <QOpenGLFunctions>
#include <QOpenGLTexture>

#include <cassert>

namespace qterrainview {

bool
TerrainRenderProgram::init()
{
  if (!m_program.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/render_terrain.vert"))
    return false;

  if (!m_program.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/render_terrain.frag"))
    return false;

  if (!m_program.link())
    return false;

  if (!m_program.bind())
    return false;

  m_mvpUniform = m_program.uniformLocation("mvp");

  assert(m_mvpUniform >= 0);

  m_positionAttrib = m_program.attributeLocation("position");

  assert(m_positionAttrib >= 0);

  m_elevationUniform = m_program.uniformLocation("elevation");

  assert(m_elevationUniform >= 0);

  m_lightDirectionUniform = m_program.uniformLocation("light_direction");

  assert(m_lightDirectionUniform >= 0);

  m_colorUniform = m_program.uniformLocation("color");

  assert(m_colorUniform >= 0);

  m_program.release();

  return true;
}

void
TerrainRenderProgram::destroy()
{}

void
TerrainRenderProgram::render(Terrain& terrain, const QMatrix4x4& view, const QMatrix4x4& proj)
{
  QOpenGLTexture* color = terrain.color();

  QOpenGLFunctions* functions = QOpenGLContext::currentContext()->functions();

  assert(functions->glGetError() == GL_NO_ERROR);

  // Setup Color Texture

  functions->glActiveTexture(GL_TEXTURE0);

  assert(functions->glGetError() == GL_NO_ERROR);

  color->bind();

  assert(functions->glGetError() == GL_NO_ERROR);

  // Setup Elevation Texture

  QOpenGLTexture* elevation = terrain.elevation();

  functions->glActiveTexture(GL_TEXTURE1);

  elevation->bind();

  // Setup Vertex Buffer

  QOpenGLBuffer* vertexBuffer = terrain.vertexBuffer();

  vertexBuffer->bind();

  // Setup Program

  m_program.bind();

  QMatrix4x4 mvp = proj * view * terrain.modelMatrix();

  QVector3D lightDirection = terrain.lightDirection();

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

  m_program.release();

  vertexBuffer->release();

  elevation->release();

  color->release();
}

} // namespace qterrainview
