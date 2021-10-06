#pragma once

#ifndef QTERRAINVIEW_QSURFACEBUILDER_H
#define QTERRAINVIEW_QSURFACEBUILDER_H

#include <memory>

class QVector2D;
class QVector3D;
class QVector3D;
class QString;

namespace qterrainview {

class QTerrainSurface;
class QTerrainView;

/// Used to build the normal map, packed PBR textures and splat maps.
class QTerrainSurfaceBuilder final
{
public:
  /// Enumerates the possible errors that can occur when either loading a texture
  /// from the file system or adding it from memory.
  enum Error
  {
    /// Indicates the operation was successful.
    NoError,
    /// Indicates a file failed to open.
    FailedToOpen,
    /// All images part of the same PBR texture must have the same size.
    SizeDoesNotMatch,
    /// A call was made outside of the begin*(), load*(), end*() order.
    CallOutOfOrder,
    /// An error occurred while uploading a texture to OpenGL.
    OpenGLError
  };

  QTerrainSurfaceBuilder() = delete;

  QTerrainSurfaceBuilder(QTerrainSurfaceBuilder&&);

  QTerrainSurfaceBuilder(const QTerrainSurfaceBuilder&) = delete;

  ~QTerrainSurfaceBuilder();

  Error beginPBRTexture();

  // From Memory

  [[nodiscard]] Error loadSplat(const float* blendData, int w, int h);

  [[nodiscard]] Error loadAlbedo(const QVector3D* rgb, int w, int h);

  [[nodiscard]] Error loadNormal(const QVector3D* xyz, int w, int h);

  [[nodiscard]] Error loadRoughness(const float* roughness, int w, int h);

  [[nodiscard]] Error loadBump(const float* bump, int w, int h);

  // From FS

  [[nodiscard]] Error loadSplatFromFile(const QString& path);

  [[nodiscard]] Error loadAlbedoFromFile(const QString& path);

  [[nodiscard]] Error loadNormalFromFile(const QString& path);

  [[nodiscard]] Error loadRoughnessFromFile(const QString& path);

  [[nodiscard]] Error loadBumpFromFile(const QString& path);

  Error finishPBRTexture();

  std::shared_ptr<QTerrainSurface> finishSurface();

private:
  friend QTerrainView;

  class Self;

  QTerrainSurfaceBuilder(Self* self);

private:
  Self* m_self = nullptr;
};

} // namespace qterrainview

#endif // QTERRAINVIEW_QSURFACEBUILDER_H
