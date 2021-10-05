#pragma once

#include <QObject>
#include <QOpenGLTexture>

namespace qterrainview {

class PBRTexture final : public QObject
{
public:
  PBRTexture();

  bool init();

  void destroy();

private:
  QOpenGLTexture m_albedo;

  QOpenGLTexture m_dispacement;

  QOpenGLTexture m_normal;

  QOpenGLTexture m_roughness;
};

} // namespace qterrainview
