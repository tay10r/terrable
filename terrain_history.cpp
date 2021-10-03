#include "terrain_history.hpp"

#include "core/tool.hpp"

TerrainHistory::TerrainHistory()
{
  m_snapshots.emplace_back(new Terrain);
}

void
TerrainHistory::undo()
{
  if (m_snapshot_index > 0)
    m_snapshot_index--;
}

void
TerrainHistory::redo()
{
  if ((m_snapshot_index + 1) < m_snapshots.size())
    m_snapshot_index++;
}

bool
TerrainHistory::applyTool(Tool& tool)
{
  auto tool_modifier = [&tool](Terrain& terrain) -> bool { return tool.execute(terrain); };

  return applyModifier(tool_modifier);
}

void
TerrainHistory::makeSnapshot()
{
  assert(m_snapshot_index < m_snapshots.size());

  m_snapshots.erase(m_snapshots.begin() + (m_snapshot_index + 1), m_snapshots.end());

  m_snapshots.emplace_back(new Terrain(*m_snapshots.back()));

  m_snapshot_index++;
}

Terrain*
TerrainHistory::currentTerrain()
{
  if (m_snapshots.empty()) {
    m_snapshots.emplace_back(new Terrain);
    m_snapshot_index = 0;
    return m_snapshots[0].get();
  }

  assert(m_snapshot_index < m_snapshots.size());

  return m_snapshots[m_snapshot_index].get();
}

