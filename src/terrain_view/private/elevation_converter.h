#pragma once

class QImage;

template<typename T>
class QVector;

namespace qterrainview {

class ElevationConverter final
{
public:
  ElevationConverter(const QImage& image);

  QVector<float> convert() const;

private:
  void convertGrayscale16(QVector<float>& elevation) const;

  void convertStandard(QVector<float>& elevation) const;

private:
  const QImage& m_image;
};

} // namespace qterrainview
