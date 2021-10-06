#include <QApplication>
#include <QColor>
#include <QMainWindow>
#include <QMatrix4x4>

#include <QTerrainView>

#include <iostream>
#include <vector>

#include <cassert>
#include <cstdlib>

using namespace qterrainview;

namespace {

void
openTerrain(QTerrainView& terrainView, const QString& terrainPath);

} // namespace

int
main(int argc, char** argv)
{
  QString terrainPath;

  if (const char* envPath = std::getenv("QTerrainView_TestTerrainPath"))
    terrainPath = envPath;

  if (terrainPath.isEmpty()) {
    std::cerr << "Must set 'QTerrainView_TestTerrainPath' environment variable first." << std::endl;
    return EXIT_FAILURE;
  }

  QApplication app(argc, argv);

  qterrainview::initResources();

  QMainWindow mainWindow;

  QTerrainView terrainView(&mainWindow);

  mainWindow.setCentralWidget(&terrainView);

  mainWindow.resize(1280, 720);

  mainWindow.show();

  QObject::connect(&terrainView, &QTerrainView::contextInitialized, [&terrainView, &terrainPath]() {
    openTerrain(terrainView, terrainPath);

    terrainView.setBackgroundColor(QColor(16, 16, 16));

    QMatrix4x4 view;

    view.lookAt(QVector3D(2, 2, 3), QVector3D(0, 0, 0), QVector3D(0, 1, 0));

    terrainView.setViewMatrix(view);

    terrainView.update();
  });

  return app.exec();
}

namespace {

void
openPBRTexture(QTerrainView& terrainView, const QString& pathPrefix)
{
  (void)terrainView;
  (void)pathPrefix;
#if 0
  QString albedoPath = pathPrefix + "albedo.png";

  QTerrainViewTexture* texture = terrainView.createTexture();

  assert(texture != nullptr);

  [[maybe_unused]] bool success =
    terrainView.loadTextureFromFile(texture, QTerrainView::Albedo, albedoPath);

  assert(success);
#endif
}

void
openTerrain(QTerrainView& terrainView, const QString& terrainPath)
{
  QString heightMapPath = terrainPath + "/height.png";

  terrainView.setMetersPerPixel(2);

  terrainView.setVerticalRange(0.2);

  terrainView.setLightDirection(-1, -1, 0);

  const bool resizeToHeightMap = true;

  [[maybe_unused]] bool success =
    terrainView.loadElevationFromFile(heightMapPath, resizeToHeightMap);

  assert(success);

  openPBRTexture(terrainView, terrainPath + "/grass_");
}

} // namespace
