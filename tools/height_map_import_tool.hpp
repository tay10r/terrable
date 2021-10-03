#pragma once

#include "core/tool.hpp"

#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>

class HeightMapImportTool final : public Tool
{
public:
  HeightMapImportTool(QWidget* parent);

  bool execute(Terrain&) override;

private:
  void executeFileDialog();

private:
  QVBoxLayout m_layout{ this };

  QLineEdit m_path{ this };

  QPushButton m_browse_button{ tr("Browse"), this };
};
