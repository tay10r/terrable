#include "open_gl_widget.hpp"

#include "terrain.h"

#include <QMatrix4x4>
#include <QOpenGLContext>
#include <QOpenGLFunctions>
#include <QOpenGLShader>
#include <QTimer>

namespace qterrainview {

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

Terrain*
OpenGLWidget::terrain()
{
  return m_terrain;
}

void
OpenGLWidget::initializeGL()
{
  QOpenGLFunctions* functions = context()->functions();

  functions->glEnable(GL_DEPTH_TEST);

  functions->glEnable(GL_MULTISAMPLE);

  m_terrainRenderProgram.init();

  m_terrain = new Terrain(this);

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
  QOpenGLFunctions* functions = context()->functions();

  functions->glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

  m_terrainRenderProgram.render(*m_terrain, m_viewMatrix, projectionMatrix());
}

QMatrix4x4
OpenGLWidget::projectionMatrix() const
{
  const float aspectRatio = float(width()) / height();

  QMatrix4x4 projectionMatrix;

  projectionMatrix.perspective(m_fieldOfView, aspectRatio, m_clipNear, m_clipFar);

  return projectionMatrix;
}

QMatrix4x4
OpenGLWidget::makeDefaultViewMatrix()
{
  QMatrix4x4 viewMatrix;

  viewMatrix.lookAt(QVector3D(2, 2, 3), QVector3D(0, 0, 0), QVector3D(0, 1, 0));

  return viewMatrix;
}

} // namespace qterrainview
