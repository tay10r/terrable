#include "tool_list.hpp"

#include "core/tool.hpp"

#include "tool_list_entry.hpp"

#include "tools/ambient_occlusion_tool.hpp"
#include "tools/height_map_import_tool.hpp"
#include "tools/scale_tool.hpp"

ToolList::ToolList(QWidget* parent)
  : QWidget(parent)
{
  m_root_layout.addWidget(&m_list_header);

  m_root_layout.addWidget(&m_list);

  m_list_layout.addStretch();

  addTool(tr("Ambient Occlusion"), new AmbientOcclusionTool(&m_list));

  addTool(tr("Scale"), new ScaleTool(&m_list));

  addTool(tr("Import Height Map"), new HeightMapImportTool(&m_list));
}

void
ToolList::addTool(const QString& label_text, Tool* tool)
{
  ToolListEntry* entry = new ToolListEntry(&m_list, label_text);

  tool->setParent(entry);

  entry->setTool(tool);

  connect(entry, &ToolListEntry::toolApplied, this, &ToolList::toolApplied);

  m_tools.emplace_back(entry);

  m_list_layout.insertWidget(m_tools.size() - 1, entry);
}
