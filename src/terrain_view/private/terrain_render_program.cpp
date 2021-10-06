#include "terrain_render_program.h"

#include "pbr_texture.h"
#include "qterrainsurface_p.h"
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

  m_albedoAndRoughnessUniform = m_program.uniformLocation("albedo_and_roughness");

  assert(m_albedoAndRoughnessUniform >= 0);

  m_program.release();

  return true;
}

void
TerrainRenderProgram::destroy()
{}

void
TerrainRenderProgram::render(Terrain& terrain, const QMatrix4x4& view, const QMatrix4x4& proj)
{
  QTerrainSurface* surface = terrain.surface();

  if (!surface)
    return;

  QOpenGLFunctions* functions = QOpenGLContext::currentContext()->functions();

  // There will be the following active textures:
  //
  //  - 1. elevation
  //  - 2. splat map
  //  - 3. pbr texture # 0 albedo and roughness
  //  - 4. pbr texture # 0 normal and bump
  //  - 5. pbr texture # 1 albedo and roughness
  //  - 6. pbr texture # 1 normal and bump
  //  - 7. pbr texture # 2 albedo and roughness
  //  - 8. pbr texture # 2 normal and bump
  //  - 9. pbr texture # 3 albedo and roughness
  //  - 10. pbr texture # 3 normal and bump

  functions->glActiveTexture(GL_TEXTURE0); // <- elevation

  // Bind elevation texture

  QOpenGLTexture* elevation = terrain.elevation();

  elevation->bind();

  // Bind Vertex Buffer

  QOpenGLBuffer* vertexBuffer = terrain.vertexBuffer();

  vertexBuffer->bind();

  // Setup Program

  m_program.bind();

  // Setup Program - Light Direction

  QVector3D lightDirection = terrain.lightDirection();

  functions->glUniform3f(
    m_lightDirectionUniform, lightDirection.x(), lightDirection.y(), lightDirection.z());

  // Setup Program - MVP Matrix

  QMatrix4x4 mvp = proj * view * terrain.modelMatrix();

  functions->glUniformMatrix4fv(m_mvpUniform, 1, GL_FALSE, mvp.constData());

  // Setup Program - Textures

  functions->glUniform1i(m_elevationUniform, 0);

  // Setup Program - Vertex Attributes

  const int vertexCount = vertexBuffer->size() / (sizeof(float) * 2);

  functions->glEnableVertexAttribArray(m_positionAttrib);

  functions->glVertexAttribPointer(m_positionAttrib, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0);

  // Draw Operations

  using PBRTextureSet = std::array<PBRTexture*, 4>;

  auto surfaceVisitor = [this, functions, vertexCount](QOpenGLTexture& /* splatMap */,
                                                       PBRTextureSet& textures) {
    (void)textures;

    PBRTexture* t0 = textures[0];

    functions->glActiveTexture(GL_TEXTURE1);

    t0->albedoAndRoughness().bind();

    functions->glUniform1i(m_albedoAndRoughnessUniform, 1);

    functions->glDrawArrays(GL_TRIANGLES, 0, vertexCount);

    assert(functions->glGetError() == GL_NO_ERROR);

    t0->albedoAndRoughness().release();

    functions->glActiveTexture(GL_TEXTURE0);
  };

  surface->m_self->visit(surfaceVisitor);

  // Cleanup

  functions->glDisableVertexAttribArray(m_positionAttrib);

  m_program.release();

  vertexBuffer->release();

  elevation->release();
}

} // namespace qterrainview
