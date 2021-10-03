#pragma once

#include "core/tool.hpp"

class AmbientOcclusionTool final : public Tool
{
public:
  AmbientOcclusionTool(QWidget* parent);

  bool execute(Terrain& terrain);

private:
};
