#pragma once

#include "core/tool.hpp"

#include <QDoubleSpinBox>
#include <QSpinBox>
#include <QVBoxLayout>

class ErosionTool : public Tool
{
public:
  ErosionTool(QWidget* widget);

  bool execute(Terrain& terrain) override;

private:
  QVBoxLayout m_layout{ this };

  QSpinBox m_iterations{ this };

  QDoubleSpinBox m_water_level_min{ this };

  QDoubleSpinBox m_water_level_max{ this };

  QDoubleSpinBox m_carry_capacity{ this };
};
