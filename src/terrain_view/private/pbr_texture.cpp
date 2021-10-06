#include "pbr_texture.h"

#include "pbr_texture_buffer.h"

#include <QOpenGLContext>
#include <QOpenGLFunctions>

namespace qterrainview {

bool
PBRTexture::init(const PBRTextureBuffer& buffer)
{
  QOpenGLFunctions* functions = QOpenGLContext::currentContext()->functions();

  // Albedo + Roughness

  if (!m_albedoRoughness.create())
    return false;

  m_albedoRoughness.bind();

  functions->glTexImage2D(GL_TEXTURE_2D,
                          0,
                          GL_RGBA,
                          buffer.width(),
                          buffer.height(),
                          0,
                          GL_RGBA,
                          GL_FLOAT,
                          buffer.albedoRoughness().data());

  functions->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  functions->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  functions->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  functions->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  m_albedoRoughness.release();

  // Normal + Bump

  if (!m_normalBump.create())
    return false;

  m_normalBump.bind();

  functions->glTexImage2D(GL_TEXTURE_2D,
                          0,
                          GL_RGBA,
                          buffer.width(),
                          buffer.height(),
                          0,
                          GL_RGBA,
                          GL_FLOAT,
                          buffer.normalBump().data());

  functions->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  functions->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  functions->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  functions->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  m_normalBump.release();

  return true;
}

} // namespace qterrainview
