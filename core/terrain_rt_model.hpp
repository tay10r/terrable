#pragma once

#include <bvh/bvh.hpp>
#include <bvh/primitive_intersectors.hpp>
#include <bvh/ray.hpp>
#include <bvh/single_ray_traverser.hpp>
#include <bvh/triangle.hpp>

#include <algorithm>
#include <vector>

#include <cassert>
#include <cstdint>

class Terrain;

class TerrainRTModel final
{
public:
  using Size = std::int64_t;

  using Vector3f = bvh::Vector3<float>;

  using Triangle = bvh::Triangle<float>;

  using Bvh = bvh::Bvh<float>;

  using Ray = bvh::Ray<float>;

  TerrainRTModel(const Terrain&);

  bool findAnyHit(const Ray& ray) const noexcept;

  Size columns() const noexcept { return m_columns; }

  Size rows() const noexcept { return m_rows; }

  Vector3f positionAt(float u, float v) const
  {
    const Triangle* tri = triangleAt(u, v);

    return tri->p0;
  }

  Vector3f normalAt(float u, float v) const
  {
    const Triangle* tri = triangleAt(u, v);

    assert(tri != nullptr);

    return -tri->n;
  }

private:
  using Vector2f = bvh::Vector<float, 2>;

  template<typename Scalar>
  static Scalar clamp(Scalar n, Scalar min, Scalar max)
  {
    return std::max(std::min(n, max), min);
  }

  static float edgeSign(const Vector2f& p, const Vector2f& v0, const Vector2f& v1)
  {
    return (p[0] - v1[0]) * (v0[1] - v1[1]) - (v0[0] - v1[0]) * (p[1] - v1[1]);
  }

  const Triangle* triangleAt(float u, float v) const
  {
    assert(m_columns > 0);
    assert(m_rows > 0);

    u = clamp(u, 0.0f, 1.0f);
    v = clamp(v, 0.0f, 1.0f);

    const float x = u * (m_columns - 1);
    const float y = v * (m_rows - 1);

    const int x0 = std::floor(x);
    const int y0 = std::floor(y);

    const int x1 = std::ceil(x);
    const int y1 = std::ceil(y);

    const Vector2f v0(x0, y0);
    const Vector2f v1(x1, y1);

    const Vector2f p(x, y);

    if (edgeSign(p, v0, v1) > 0)
      return &m_triangles[(((y0 * m_columns) + x0) * 2) + 0];
    else
      return &m_triangles[(((y0 * m_columns) + x0) * 2) + 1];
  }

private:
  std::vector<Triangle> m_triangles;

  Bvh m_bvh;

  Size m_rows = 0;

  Size m_columns = 0;
};

inline bool
TerrainRTModel::findAnyHit(const Ray& ray) const noexcept
{
  using Intersector = bvh::ClosestPrimitiveIntersector<Bvh, Triangle>;

  Intersector intersector(m_bvh, m_triangles.data());

  bvh::SingleRayTraverser<Bvh> traverser(m_bvh);

  return traverser.traverse(ray, intersector).has_value();
}
