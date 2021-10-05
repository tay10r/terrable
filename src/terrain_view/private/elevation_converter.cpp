#include "elevation_converter.h"

#include <QImage>
#include <QVector>

#include <cstdint>

namespace qterrainview {

ElevationConverter::ElevationConverter(const QImage& image)
  : m_image(image)
{}

QVector<float>
ElevationConverter::convert() const
{
  QVector<float> elevation(m_image.width() * m_image.height());

  if (m_image.format() == QImage::Format_Grayscale16)
    convertGrayscale16(elevation);
  else
    convertStandard(elevation);

  return elevation;
}

void
ElevationConverter::convertGrayscale16(QVector<float>& elevation) const
{
  const std::uint16_t* data = (const std::uint16_t*)m_image.bits();

#pragma omp parallel for

  for (int i = 0; i < elevation.size(); i++)
    elevation[i] = float(data[i]) / 65535.0f;
}

void
ElevationConverter::convertStandard(QVector<float>& elevation) const
{
  for (int i = 0; i < elevation.size(); i++) {
    const int x = i % m_image.width();
    const int y = i / m_image.width();

    elevation[i] = qGray(m_image.pixel(x, y)) / 255.0f;
  }
}

} // namespace qterrainview
