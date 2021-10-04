#pragma once

#include "core/tool.hpp"

#include <QSpinBox>
#include <QVBoxLayout>

class AmbientOcclusionTool final : public Tool
{
public:
  AmbientOcclusionTool(QWidget* parent);

  bool execute(Terrain& terrain);

private:
  QVBoxLayout m_layout{ this };

  QSpinBox m_rays_per_pixel{ this };
};
