#include "ambient_occlusion_tool.hpp"

#include "core/terrain.hpp"
#include "core/terrain_rt_model.hpp"

#include <QProgressDialog>

#include <random>

#include <QDebug>

namespace {

template<typename Rng>
bvh::Vector3<float>
sampleHemisphere(Rng& rng, const bvh::Vector3<float>& normal)
{
  std::uniform_real_distribution<float> dist(0, 1);

  for (int i = 0; i < 128; i++) {
    bvh::Vector3<float> d(dist(rng), dist(rng), dist(rng));

    if ((bvh::dot(d, d) <= 1) && (bvh::dot(d, normal) > 0))
      return bvh::normalize(d);
  }

  return normal;
}

QVector3D
computeCell(const TerrainRTModel& rt_model, Texture::Size x, Texture::Size y, int rays_per_pixel)
{
  std::mt19937 rng((y * rt_model.columns()) + x);

  std::uniform_real_distribution<float> uv_dist(0, 1);

  const float u_scale = 1.0f / rt_model.columns();
  const float v_scale = 1.0f / rt_model.rows();

  const float shadow_bias = 0.0001;

  using Vector3f = bvh::Vector3<float>;

  float occlusion_total = 0;

  for (int i = 0; i < rays_per_pixel; i++) {

    const float u = (int(x) + uv_dist(rng)) * u_scale;
    const float v = (int(y) + uv_dist(rng)) * v_scale;

    const Vector3f position = rt_model.positionAt(u, v);

    const Vector3f normal = rt_model.normalAt(u, v);

    const Vector3f ray_direction = sampleHemisphere(rng, normal);

    const Vector3f ray_origin = position + (ray_direction * shadow_bias);

    bvh::Ray ray(ray_origin, ray_direction);

    if (rt_model.findAnyHit(ray))
      occlusion_total++;
  }

  const float ao_level = 1.0 - (occlusion_total / rays_per_pixel);

  return QVector3D(ao_level, ao_level, ao_level);
}

void
computeRow(Texture& ao_texture, Texture::Size y, const TerrainRTModel& rt_model, int rays_per_pixel)
{
#pragma omp parallel for

  for (Texture::Size x = 0; x < ao_texture.width(); x++)
    ao_texture(x, y) = computeCell(rt_model, x, y, rays_per_pixel);
}

} // namespace

AmbientOcclusionTool::AmbientOcclusionTool(QWidget* parent)
  : Tool(parent)
{
  m_layout.addWidget(&m_rays_per_pixel);

  m_rays_per_pixel.setMinimum(1);
  m_rays_per_pixel.setMaximum(65536);
  m_rays_per_pixel.setValue(1024);
}

bool
AmbientOcclusionTool::execute(Terrain& terrain)
{
  TerrainRTModel rt_model(terrain);

  QProgressDialog progress_dialog(tr("Working"), tr("Cancel"), 0, terrain.rows(), this);

  progress_dialog.setWindowModality(Qt::WindowModal);

  Texture ao_texture(terrain.columns(), terrain.rows());

  const int rays_per_pixel = m_rays_per_pixel.value();

  for (Texture::Size y = 0; y < ao_texture.height(); y++) {

    if (progress_dialog.wasCanceled())
      return false;

    computeRow(ao_texture, y, rt_model, rays_per_pixel);

    progress_dialog.setValue(y);
  }

  const std::string texture_name = tr("Ambient Occlusion").toStdString();

  terrain.defineTexture(texture_name, std::move(ao_texture));

  terrain.setCurrentTexture(texture_name);

  return true;
}
