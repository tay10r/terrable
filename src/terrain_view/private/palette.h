#pragma once

#include <QObject>

#include "pbr_texture.h"

namespace qterrainview {

class Palette final : public QObject
{
public:
  Palette(QObject* parent);

private:
  PBRTexture texture[4];
};

} // namespace qterrainview
