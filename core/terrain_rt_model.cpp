#include "terrain_rt_model.hpp"

#include "core/terrain.hpp"

#include <bvh/sweep_sah_builder.hpp>

TerrainRTModel::TerrainRTModel(const Terrain& terrain)
{
  m_rows = terrain.rows();

  m_columns = terrain.columns();

  const Terrain::Vertex* vertices = terrain.vertexData();

  const Terrain::Size vertex_count = terrain.vertexCount();

  using Vector3f = bvh::Vector3<float>;

  m_triangles.resize(vertex_count / 3);

#pragma omp parallel for

  for (Terrain::Size i = 0; i < vertex_count; i += 6) {

    const QVector3D& p0 = vertices[i + 0].position;
    const QVector3D& p1 = vertices[i + 1].position;
    const QVector3D& p2 = vertices[i + 2].position;

    const QVector3D& p3 = vertices[i + 3].position;
    const QVector3D& p4 = vertices[i + 4].position;
    const QVector3D& p5 = vertices[i + 5].position;

    const Vector3f v0(p0[0], p0[1], p0[2]);
    const Vector3f v1(p1[0], p1[1], p1[2]);
    const Vector3f v2(p2[0], p2[1], p2[2]);

    const Vector3f v3(p3[0], p3[1], p3[2]);
    const Vector3f v4(p4[0], p4[1], p4[2]);
    const Vector3f v5(p5[0], p5[1], p5[2]);

    const Terrain::Size j = i / 3;

    m_triangles[j + 0] = Triangle(v0, v1, v2);
    m_triangles[j + 1] = Triangle(v3, v4, v5);
  }

  auto [bboxes, centers] =
    bvh::compute_bounding_boxes_and_centers(m_triangles.data(), m_triangles.size());

  auto global_bbox = bvh::compute_bounding_boxes_union(bboxes.get(), m_triangles.size());

  bvh::SweepSahBuilder<Bvh> builder(m_bvh);

  builder.build(global_bbox, bboxes.get(), centers.get(), m_triangles.size());
}
