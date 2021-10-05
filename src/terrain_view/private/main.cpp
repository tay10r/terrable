#include <QApplication>
#include <QMainWindow>
#include <QMatrix4x4>

#include <QTerrainView>

#include <iostream>
#include <vector>

#include <cassert>
#include <cstdlib>

namespace {

struct Texture final
{
  std::vector<float> rgba;
  int w = 0;
  int h = 0;
};

Texture
makeInvalidTexture()
{
  Texture texture;
  texture.rgba.resize(4);
  texture.w = 1;
  texture.h = 1;
  texture.rgba[0] = 1;
  texture.rgba[1] = 0;
  texture.rgba[2] = 0;
  texture.rgba[3] = 1;
  return texture;
}

Texture
openTexture(const char* path);

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

  QMainWindow mainWindow;

  QTerrainView terrainView(&mainWindow);

  mainWindow.setCentralWidget(&terrainView);

  mainWindow.resize(1280, 720);

  mainWindow.show();

  QObject::connect(&terrainView, &QTerrainView::contextInitialized, [&terrainView, &terrainPath]() {
    QString heightMapPath = terrainPath + "/height.png";

    const bool resizeToHeightMap = true;

    terrainView.setMetersPerPixel(2);

    terrainView.setVerticalRange(0.2);

    [[maybe_unused]] bool success =
      terrainView.loadElevationFromFile(heightMapPath, resizeToHeightMap);

    assert(success);

    QMatrix4x4 view;

    view.lookAt(QVector3D(2, 2, 3), QVector3D(0, 0, 0), QVector3D(0, 1, 0));

    terrainView.setViewMatrix(view);

    terrainView.update();
  });

  return app.exec();
}

namespace {

Texture
openTexture(const char* path)
{
  (void)path;

  return makeInvalidTexture();
}

} // namespace
