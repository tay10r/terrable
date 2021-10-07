#include <QApplication>
#include <QColor>
#include <QMainWindow>
#include <QMatrix4x4>
#include <QSurfaceFormat>

#include <qterrainview/QTerrainSurfaceBuilder>
#include <qterrainview/QTerrainView>

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
  QSurfaceFormat surfaceFormat = QSurfaceFormat::defaultFormat();
  surfaceFormat.setSamples(4);
  QSurfaceFormat::setDefaultFormat(surfaceFormat);

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

    // view.lookAt(QVector3D(0.2, 0.4, 0.3), QVector3D(0.1, 0, 0), QVector3D(0, 1, 0));
    view.lookAt(QVector3D(1, 1, 2), QVector3D(0, 0, 0), QVector3D(0, 1, 0));

    terrainView.setViewMatrix(view);

    terrainView.update();
  });

  return app.exec();
}

namespace {

void
openPBRTexture(QTerrainSurfaceBuilder& surfaceBuilder, const QString& pathPrefix)
{
  QString albedoPath = pathPrefix + "albedo.png";

  using Error = QTerrainSurfaceBuilder::Error;

  surfaceBuilder.beginPBRTexture();

  [[maybe_unused]] Error error = surfaceBuilder.loadAlbedoFromFile(pathPrefix + "albedo.png");

  assert(error == QTerrainSurfaceBuilder::NoError);

  error = surfaceBuilder.loadNormalFromFile(pathPrefix + "normal.png");

  assert(error == QTerrainSurfaceBuilder::NoError);

  error = surfaceBuilder.loadSplatFromFile(pathPrefix + "splat.png");

  assert(error == QTerrainSurfaceBuilder::NoError);

  error = surfaceBuilder.finishPBRTexture();

  assert(error == QTerrainSurfaceBuilder::NoError);
}

void
openTerrain(QTerrainView& terrainView, const QString& terrainPath)
{
  QString heightMapPath = terrainPath + "/height.png";

  terrainView.setMetersPerPixel(2);

  terrainView.setVerticalRange(0.3);

  terrainView.setLightDirection(-0.5, -1, 0);

  const bool resizeToHeightMap = true;

  [[maybe_unused]] bool success =
    terrainView.loadElevationFromFile(heightMapPath, resizeToHeightMap);

  assert(success);

  QTerrainSurfaceBuilder surfaceBuilder = terrainView.surfaceBuilder();

  openPBRTexture(surfaceBuilder, terrainPath + "/grass_");

  openPBRTexture(surfaceBuilder, terrainPath + "/rock_");

  std::shared_ptr<QTerrainSurface> surface = surfaceBuilder.finishSurface();

  terrainView.setSurface(surface);
}

} // namespace
