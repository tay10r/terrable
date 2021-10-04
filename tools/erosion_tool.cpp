#include "tools/erosion_tool.hpp"

#include "core/terrain.hpp"

#include <QProgressDialog>

#include <TinyErode.h>

#include <random>

#include <cstdint>

namespace {

using Size = std::int64_t;

std::vector<float>
generateWater(Size w, Size h, float min, float max)
{
  std::mt19937 rng(w * h);

  std::uniform_real_distribution<float> dist(min, max);

  std::vector<float> water(w * h);

  for (Size i = 0; i < (w * h); i++)
    water[i] = dist(rng);

  return water;
}

} // namespace

ErosionTool::ErosionTool(QWidget* parent)
  : Tool(parent)
{
  m_iterations.setMaximum(0xffffff);

  m_iterations.setValue(1024);

  m_water_level_min.setValue(0.1);

  m_water_level_max.setValue(0.11);

  m_carry_capacity.setSingleStep(0.01);
  m_carry_capacity.setMinimum(0.0);
  m_carry_capacity.setMaximum(1.0);
  m_carry_capacity.setValue(0.01);

  m_layout.addWidget(&m_iterations);
  m_layout.addWidget(&m_water_level_min);
  m_layout.addWidget(&m_water_level_max);
  m_layout.addWidget(&m_carry_capacity);
}

bool
ErosionTool::execute(Terrain& terrain)
{
  const int iterations = m_iterations.value();

  QProgressDialog progress_dialog(tr("Working"), tr("Cancel"), 0, iterations, this);

  progress_dialog.setWindowModality(Qt::WindowModal);

  const Size w = terrain.columns();
  const Size h = terrain.rows();

  const float metersPerX = 1000.0f / w;
  const float metersPerY = 1000.0f / h;

  const float water_level_min = std::min(m_water_level_min.value(), m_water_level_max.value());
  const float water_level_max = std::max(m_water_level_min.value(), m_water_level_max.value());

  const float carry_capacity = m_carry_capacity.value();

  std::vector<float> water = generateWater(w, h, water_level_min, water_level_max);

  std::vector<float> elevation = terrain.getElevation();

  auto getHeight = [&elevation, w](int x, int y) { return elevation[(y * w) + x]; };

  auto getWater = [&water, w](int x, int y) { return water[(y * w) + x]; };

  auto addWater = [&water, w](int x, int y, float waterDelta) -> float {
    return water[(y * w) + x] = std::max(0.0f, water[(y * w) + x] + waterDelta);
  };

  auto addHeight = [&elevation, w](int x, int y, float deltaHeight) {
    elevation[(y * w) + x] += deltaHeight;
  };

  auto carryCapacity = [carry_capacity](int, int) -> float { return carry_capacity; };

  auto deposition = [](int, int) -> float { return 0.1; };

  auto erosion = [](int, int) -> float { return 0.1; };

  auto evaporation = [](int, int) -> float { return 0.1; };

  TinyErode::Simulation simulation(w, h);

  simulation.SetMetersPerX(metersPerX);
  simulation.SetMetersPerY(metersPerY);

  for (int i = 0; i < iterations; i++) {

    if (progress_dialog.wasCanceled())
      return false;

    simulation.ComputeFlowAndTilt(getHeight, getWater);

    simulation.TransportWater(addWater);

    simulation.TransportSediment(carryCapacity, deposition, erosion, addHeight);

    simulation.Evaporate(addWater, evaporation);

    progress_dialog.setValue(i);
  }

  simulation.TerminateRainfall(addHeight);

  terrain.setElevation(elevation);

  return true;
}
