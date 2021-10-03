#include "scale_tool.hpp"

#include "terrain.hpp"

#include <QMatrix4x4>

ScaleTool::ScaleTool(QWidget* parent)
  : Tool(parent)
{
  m_layout.addWidget(&m_x_spin_box);
  m_layout.addWidget(&m_y_spin_box);
  m_layout.addWidget(&m_z_spin_box);

  m_x_spin_box.setValue(1);
  m_y_spin_box.setValue(1);
  m_z_spin_box.setValue(1);
}

bool
ScaleTool::execute(Terrain& terrain)
{
  Terrain::Vertex* vertices = terrain.vertexData();

  QMatrix4x4 scale_matrix;

  scale_matrix.scale(m_x_spin_box.value(), m_y_spin_box.value(), m_z_spin_box.value());

#pragma omp parallel for

  for (Terrain::Size i = 0; i < terrain.vertexCount(); i++) {

    Terrain::Vertex& vertex = vertices[i];

    QVector4D p(vertex.position[0], vertex.position[1], vertex.position[2], 1);

    QVector4D new_p = scale_matrix * p;

    vertex.position[0] = new_p[0];
    vertex.position[1] = new_p[1];
    vertex.position[2] = new_p[2];
  }

  return true;
}
