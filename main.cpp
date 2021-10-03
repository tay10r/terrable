#include <QApplication>
#include <QMainWindow>
#include <QVBoxLayout>

#include "terrain.hpp"
#include "terrain_view.hpp"

#include <cassert>

namespace {

class MainWindow final : public QMainWindow
{
public:
  MainWindow()
  {
    resize(1280, 720);

    setCentralWidget(&m_central_widget);

    m_central_widget.setLayout(&m_layout);

    m_layout.addWidget(&m_terrain_view);
  }

  TerrainView* terrainView() { return &m_terrain_view; }

private:
  QWidget m_central_widget{ this };

  QVBoxLayout m_layout{ &m_central_widget };

  TerrainView m_terrain_view{ &m_central_widget };
};

class TerrainUpdater final : public QObject
{
public:
  TerrainUpdater(TerrainView* terrain_view, Terrain* terrain)
    : m_terrain_view(terrain_view)
    , m_terrain(terrain)
  {}

public slots:
  void update() { m_terrain_view->loadTerrain(*m_terrain); }

private:
  TerrainView* m_terrain_view;

  Terrain* m_terrain;
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

  TerrainView* terrain_view = main_window.terrainView();

  TerrainUpdater terrain_updater(terrain_view, &terrain);

  success = QObject::connect(
    terrain_view, &TerrainView::contextInitialized, &terrain_updater, &TerrainUpdater::update);

  assert(success);

  return app.exec();
}
