#pragma once

#include <QObject>
#include <QOpenGLTexture>

namespace qterrainview {

class PBRTextureBuffer;

class PBRTexture final : public QObject
{
public:
  bool init(const PBRTextureBuffer&);

  void destroy();

private:
  QOpenGLTexture m_albedoRoughness{ QOpenGLTexture::Target2D };

  QOpenGLTexture m_normalBump{ QOpenGLTexture::Target2D };
};

} // namespace qterrainview
