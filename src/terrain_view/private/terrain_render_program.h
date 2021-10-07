#pragma once

#include <QOpenGLShaderProgram>

namespace qterrainview {

class Terrain;

class TerrainRenderProgram final
{
public:
  bool init();

  void destroy();

  void render(Terrain& terrain, const QMatrix4x4& view, const QMatrix4x4& proj);

private:
  QOpenGLShaderProgram m_program;

  // From Vertex Shader

  int m_modelUniform = -1;

  int m_mvpUniform = -1;

  int m_cameraPositionUniform = -1;

  int m_elevationUniform = -1;

  int m_positionAttrib = -1;

  // From Fragment Shader

  int m_lightDirectionUniform = -1;

  int m_splatMapUniform = -1;

  int m_textureSizesUniform = -1;

  int m_albedoAndRoughnessUniform[4]{ -1, -1, -1, -1 };

  int m_normalAndBumpUniform[4]{ -1, -1, -1, -1 };
};

} // namespace qterrainview
