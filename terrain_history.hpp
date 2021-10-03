#pragma once

#include <QObject>

#include "terrain.hpp"

#include <memory>
#include <vector>

class Tool;

class TerrainHistory final : public QObject
{
  Q_OBJECT
public:
  TerrainHistory();

  void undo();

  void redo();

  bool applyTool(Tool& tool);

  template<typename Modifier>
  bool applyModifier(Modifier modifier);

signals:
  void terrainChanged(Terrain& terrain);

private:
  Terrain* currentTerrain();

  void makeSnapshot();

private:
  std::vector<std::unique_ptr<Terrain>> m_snapshots;

  size_t m_snapshot_index = 0;
};

template<typename Modifier>
bool
TerrainHistory::applyModifier(Modifier modifier)
{
  makeSnapshot();

  Terrain* terrain = currentTerrain();

  const bool success = modifier(*terrain);

  if (success)
    emit terrainChanged(*terrain);

  return success;
}
