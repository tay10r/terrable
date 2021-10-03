#include "height_map_import_tool.hpp"

#include "terrain.hpp"

#include <QFileDialog>

HeightMapImportTool::HeightMapImportTool(QWidget* parent)
  : Tool(parent)
{
  m_path.setPlaceholderText(tr("Enter a path or hit 'Browse'"));

  m_layout.addWidget(&m_path);

  m_layout.addWidget(&m_browse_button);

  connect(&m_browse_button, &QPushButton::clicked, this, &HeightMapImportTool::executeFileDialog);
}

bool
HeightMapImportTool::execute(Terrain& terrain)
{
  return terrain.openFromHeightMap(m_path.text());
}

void
HeightMapImportTool::executeFileDialog()
{
  QString filePath = QFileDialog::getOpenFileName(
    this, "Open an existing height map.", QString(), tr("Images (*.png *.xpm *jpg)"));

  m_path.setText(filePath);
}
