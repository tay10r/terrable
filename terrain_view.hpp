#pragma once

#include <QComboBox>
#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <QMatrix4x4>
#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QOpenGLWidget>

#include "core/camera.hpp"

class QMouseEvent;
class QWheelEvent;

class Terrain;

class TerrainViewHeader : public QFrame
{
  Q_OBJECT
public:
  TerrainViewHeader(QWidget* parent);

signals:
  void textureSelectionChanged(const QString& texture_name);

public slots:
  void updateTextureOptions(const Terrain&);

private:
  QHBoxLayout m_layout{ this };

  QLabel m_texture_selection_label{ tr("Current Texture"), this };

  QComboBox m_texture_selection{ this };
};

class TerrainView : public QOpenGLWidget
{
  Q_OBJECT
public:
  TerrainView(QWidget* parent, Camera& camera);

  void loadTerrain(Terrain&);

signals:
  void contextInitialized();

protected:
  void mouseMoveEvent(QMouseEvent*) override;

  void wheelEvent(QWheelEvent*) override;

  void initializeGL() override;

  void paintGL() override;

  void resizeGL(int, int) override;

  QMatrix4x4 makeMVPMatrix() const;

private:
  Camera& m_camera;

  bool m_context_initialized = false;

  int m_terrain_vertex_count = 0;

  int m_mvp_location = -1;

  int m_position_location = -1;

  int m_texture_coordinates_location = -1;

  float m_aspect_ratio = 1;

  QOpenGLShaderProgram m_render_terrain_program;

  QOpenGLBuffer m_terrain_vertex_buffer;

  GLuint m_terrain_color_texture = 0;

  QMatrix4x4 m_view_matrix;
};

class TerrainViewContainer : public QFrame
{
  Q_OBJECT
public:
  TerrainViewContainer(QWidget* parent);

  TerrainView* terrainView() { return &m_terrain_view; }

  TerrainViewHeader* terrainViewHeader() { return &m_terrain_view_header; }

  void terrainUpdate(Terrain&);

public slots:
  void onFocusChange(QWidget* old, QWidget* now);

signals:
  void contextInitialized();

  void textureSelectionChanged(const QString& name);

private:
  static std::unique_ptr<Camera> createDefaultCamera(QObject* parent);

private:
  std::unique_ptr<Camera> m_camera = createDefaultCamera(this);

  QVBoxLayout m_layout{ this };

  TerrainViewHeader m_terrain_view_header{ this };

  TerrainView m_terrain_view{ this, *m_camera };
};
