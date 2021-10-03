#pragma once

#include "tool.hpp"

#include <QDoubleSpinBox>
#include <QPushButton>
#include <QVBoxLayout>

class ScaleTool final : public Tool
{
public:
  ScaleTool(QWidget* parent);

  bool execute(Terrain&) override;

private:
  QVBoxLayout m_layout{ this };

  QDoubleSpinBox m_x_spin_box{ this };
  QDoubleSpinBox m_y_spin_box{ this };
  QDoubleSpinBox m_z_spin_box{ this };
};
