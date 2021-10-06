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

  int m_lightDirectionUniform = -1;

  int m_mvpUniform = -1;

  int m_elevationUniform = -1;

  int m_albedoAndRoughnessUniform = -1;

  int m_positionAttrib = -1;
};

} // namespace qterrainview
