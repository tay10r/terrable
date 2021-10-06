#include <qterrainview/qterrainsurface.h>

#include "qterrainsurface_p.h"

namespace qterrainview {

QTerrainSurface::QTerrainSurface(Self* self)
  : m_self(self)
{}

QTerrainSurface::~QTerrainSurface()
{
  delete m_self;
}

} // namespace qterrainview
