#include <QApplication>
#include <QFile>
#include <QMainWindow>
#include <QSplitter>
#include <QSurfaceFormat>
#include <QToolBar>
#include <QVBoxLayout>

#include "core/terrain.hpp"

#include "terrain_history.hpp"
#include "terrain_view.hpp"
#include "tool_list.hpp"

#include <cassert>

#ifndef NDEBUG
#define INIT_TERRAIN_PATH ":/images/init_terrain_low_resolution.png"
#else
#define INIT_TERRAIN_PATH ":/images/init_terrain.png"
#endif

namespace {

class MainWindow final : public QMainWindow
{
public:
  MainWindow()
  {
    resize(1280, 720);

    setWindowTitle("Terrable");
  }
};

class TerrainInitializer final : public QObject
{
public:
  TerrainInitializer(TerrainHistory& terrain_history)
    : m_terrain_history(terrain_history)
  {}

public slots:
  void initializeTerrain()
  {
    auto modifier = [](Terrain& terrain) -> bool {
      [[maybe_unused]] const bool success = terrain.openFromHeightMap(INIT_TERRAIN_PATH);

      assert(success);

      return true;
    };

    m_terrain_history.applyModifier(modifier);
  }

private:
  TerrainHistory& m_terrain_history;
};

} // namespace

int
main(int argc, char** argv)
{
  QSurfaceFormat surface_format = QSurfaceFormat::defaultFormat();
  surface_format.setSamples(4);
  QSurfaceFormat::setDefaultFormat(surface_format);

  QApplication app(argc, argv);

  QFile file(":/stylesheet.css");

  file.open(QFile::ReadOnly);

  QString styleSheet(QLatin1String(file.readAll()));

  app.setStyleSheet(styleSheet);

  MainWindow main_window;

  TerrainHistory terrain_history;

  main_window.show();

  QSplitter splitter(&main_window);

  main_window.setCentralWidget(&splitter);

  TerrainViewContainer terrain_view_container(&splitter);

  splitter.addWidget(&terrain_view_container);

  QObject::connect(&app,
                   &QApplication::focusChanged,
                   &terrain_view_container,
                   &TerrainViewContainer::onFocusChange);

  ToolList tool_list(&splitter);

  splitter.addWidget(&tool_list);

  QObject::connect(
    &tool_list, &ToolList::toolApplied, &terrain_history, &TerrainHistory::applyTool);

  QObject::connect(&terrain_history,
                   &TerrainHistory::terrainChanged,
                   &terrain_view_container,
                   &TerrainViewContainer::terrainUpdate);

  TerrainInitializer terrain_initializer(terrain_history);

  [[maybe_unused]] bool success = QObject::connect(&terrain_view_container,
                                                   &TerrainViewContainer::contextInitialized,
                                                   &terrain_initializer,
                                                   &TerrainInitializer::initializeTerrain);

  assert(success);

  splitter.setSizes({ 80, 20 });

  return app.exec();
}
