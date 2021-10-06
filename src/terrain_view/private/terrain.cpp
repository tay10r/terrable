#include "terrain.h"

#include <qterrainview/QTerrainSurface>

#include <QOpenGLContext>
#include <QOpenGLFunctions>
#include <QVector2D>

#include <cassert>

namespace qterrainview {

namespace {

const float g_initialTerrainElevation[4]{ 0, 0, 0, 0 };

const int g_initialTerrainWidth = 2;

const int g_initialTerrainHeight = 2;

std::vector<float>
generateTexCoords(int w, int h)
{
  const int floatsPerVertex = 2;

  const int verticesPerPixel = 6;

  std::vector<float> texCoords(w * h * floatsPerVertex * verticesPerPixel);

  auto toUV = [w, h](int x, int y) -> QVector2D {
    const float u = x / float(w);
    const float v = y / float(h);
    return QVector2D(u, v);
  };

  for (int i = 0; i < (w * h); i++) {

    const int vertexOffset = i * floatsPerVertex * verticesPerPixel;

    const int x = i % w;
    const int y = i / w;

    const QVector2D p00(toUV(x + 0, y + 0));
    const QVector2D p10(toUV(x + 1, y + 0));
    const QVector2D p01(toUV(x + 0, y + 1));
    const QVector2D p11(toUV(x + 1, y + 1));

    float* vertices = &texCoords[vertexOffset];

    // Triangle 0

    vertices[0] = p00.x();
    vertices[1] = p00.y();

    vertices[2] = p01.x();
    vertices[3] = p01.y();

    vertices[4] = p10.x();
    vertices[5] = p10.y();

    // Triangle 1

    vertices[6] = p10.x();
    vertices[7] = p10.y();

    vertices[8] = p01.x();
    vertices[9] = p01.y();

    vertices[10] = p11.x();
    vertices[11] = p11.y();
  }

  return texCoords;
}

} // namespace

Terrain::Terrain(QObject* parent)
  : QObject(parent)
{}

bool
Terrain::init()
{
  if (!m_vertexBuffer.create())
    return false;

  if (!m_elevation.create())
    return false;

  if (!resize(g_initialTerrainWidth, g_initialTerrainHeight))
    return false;

  if (!setElevation(g_initialTerrainElevation, g_initialTerrainWidth, g_initialTerrainHeight))
    return false;

  return true;
}

void
Terrain::destroy()
{
  if (m_vertexBuffer.isCreated())
    m_vertexBuffer.destroy();

  if (m_elevation.isCreated())
    m_elevation.destroy();
}

bool
Terrain::resize(Size w, Size h)
{
  if ((w < 2) || (h < 2))
    return false;

  const Size floatsPerVertex = 2;

  const Size verticesPerPixel = 6;

  Size bufferSize = sizeof(float) * floatsPerVertex * verticesPerPixel * w * h;

  if (!m_vertexBuffer.bind())
    return false;

  m_vertexBuffer.allocate(bufferSize);

  std::vector<float> texCoords = generateTexCoords(w, h);

  m_vertexBuffer.write(0, texCoords.data(), texCoords.size() * sizeof(float));

  m_vertexBuffer.release();

  assert(QOpenGLContext::currentContext()->functions()->glGetError() == GL_NO_ERROR);

  return true;
}

void
Terrain::setSurface(const std::shared_ptr<QTerrainSurface>& surface)
{
  m_surface = surface;
}

bool
Terrain::setElevation(const float* data, Size w, Size h)
{
  m_elevation.bind();

  QOpenGLFunctions* functions = QOpenGLContext::currentContext()->functions();

  functions->glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, w, h, 0, GL_RED, GL_FLOAT, data);

  functions->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  functions->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  functions->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  functions->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  m_elevation.release();

  assert(functions->glGetError() == GL_NO_ERROR);

  return true;
}

} // namespace qterrainview
