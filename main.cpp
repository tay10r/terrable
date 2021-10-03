#include <QApplication>
#include <QMainWindow>
#include <QVBoxLayout>

#include "orbit_camera.hpp"
#include "terrain.hpp"
#include "terrain_view.hpp"

#include <cassert>

namespace {

class MainWindow final : public QMainWindow
{
public:
  MainWindow() { resize(1280, 720); }
};

class TerrainUpdater final : public QObject
{
public:
  TerrainUpdater(TerrainView& terrain_view, Terrain& terrain)
    : m_terrain_view(terrain_view)
    , m_terrain(terrain)
  {}

public slots:
  void update() { m_terrain_view.loadTerrain(m_terrain); }

private:
  TerrainView& m_terrain_view;

  Terrain& m_terrain;
};

} // namespace

int
main(int argc, char** argv)
{
  QApplication app(argc, argv);

  MainWindow main_window;

  Terrain terrain;

  bool success = terrain.openFromHeightMap(":/init_terrain.png");

  assert(success);

  main_window.show();

  OrbitCamera orbit_camera;

  TerrainView terrain_view(&main_window, orbit_camera);

  QObject::connect(
    &app, &QApplication::focusChanged, [&terrain_view, &orbit_camera](QWidget* old, QWidget* now) {
      if (now == &terrain_view)
        orbit_camera.gotFocus();
      else if (old == &terrain_view)
        orbit_camera.lostFocus();
    });

  QObject::connect(
    &orbit_camera, &OrbitCamera::cameraUpdate, [&terrain_view]() { terrain_view.update(); });

  main_window.setCentralWidget(&terrain_view);

  TerrainUpdater terrain_updater(terrain_view, terrain);

  success = QObject::connect(
    &terrain_view, &TerrainView::contextInitialized, &terrain_updater, &TerrainUpdater::update);

  assert(success);

  return app.exec();
}
