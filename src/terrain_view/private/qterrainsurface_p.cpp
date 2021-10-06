#include "qterrainsurface_p.h"

#include "splat_map_buffer.h"

#include <QOpenGLContext>
#include <QOpenGLFunctions>
#include <QOpenGLWidget>

#include <cassert>

namespace qterrainview {

namespace {

std::vector<float>
interleave(const std::vector<float>& a,
           const std::vector<float>& b,
           const std::vector<float>& c,
           const std::vector<float>& d)
{
  // These should all be the same size. This is just in case error checking at a higher level fails.

  size_t max_size = std::max(std::max(std::max(a.size(), b.size()), c.size()), d.size());
  size_t min_size = std::min(std::min(std::min(a.size(), b.size()), c.size()), d.size());

  std::vector<float> buffer(max_size * 4);

  for (size_t i = 0; i < min_size; i++) {
    buffer[(i * 4) + 0] = a[i];
    buffer[(i * 4) + 1] = b[i];
    buffer[(i * 4) + 2] = c[i];
    buffer[(i * 4) + 3] = d[i];
  }

  return buffer;
}

} // namespace

QTerrainSurface::Self::Self(QOpenGLWidget& openGLWidget,
                            std::vector<std::unique_ptr<PBRTexture>>&& textures,
                            const SplatMapBuffer& splatMapBuffer)
  : m_openGLWidget(openGLWidget)
  , m_textures(std::move(textures))
{
  assert((splatMapBuffer.size() % 4) == 0);

  openGLWidget.makeCurrent();

  QOpenGLFunctions* functions = QOpenGLContext::currentContext()->functions();

  for (size_t i = 0; i < splatMapBuffer.size(); i += 4) {

    const std::vector<float>& a = splatMapBuffer[i + 0];
    const std::vector<float>& b = splatMapBuffer[i + 1];
    const std::vector<float>& c = splatMapBuffer[i + 2];
    const std::vector<float>& d = splatMapBuffer[i + 3];

    std::vector<float> buffer = interleave(a, b, c, d);

    std::unique_ptr<QOpenGLTexture> splatTexture(new QOpenGLTexture(QOpenGLTexture::Target2D));

    if (!splatTexture->create())
      continue;

    splatTexture->bind();

    functions->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    functions->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    functions->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    functions->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    functions->glTexImage2D(GL_TEXTURE_2D,
                            0,
                            GL_RGBA,
                            splatMapBuffer.width(),
                            splatMapBuffer.height(),
                            0,
                            GL_RGBA,
                            GL_FLOAT,
                            buffer.data());

    splatTexture->release();

    m_splatMaps.emplace_back(std::move(splatTexture));
  }

  openGLWidget.doneCurrent();
}

QTerrainSurface::Self::~Self()
{
  m_openGLWidget.makeCurrent();

  m_textures.clear();

  m_splatMaps.clear();

  m_openGLWidget.doneCurrent();
}

} // namespace qterrainview
