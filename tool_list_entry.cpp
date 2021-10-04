#include "tool_list_entry.hpp"

#include "core/tool.hpp"

#include <cassert>

ToolListEntryHeader::ToolListEntryHeader(const QString& name, QWidget* parent)
  : QPushButton(name, parent)
{}

ToolListEntry::ToolListEntry(QWidget* parent, const QString& name)
  : QFrame(parent)
  , m_label(name, this)
{
#if 0
  m_apply_button.setFlat(true);

  m_label.setFlat(true);
#endif

  m_layout.addWidget(&m_label);

  m_layout.addWidget(&m_apply_button);

  m_apply_button.hide();

  connect(&m_label, &QPushButton::clicked, this, &ToolListEntry::toggleToolVisibility);

  connect(&m_apply_button, &QPushButton::clicked, this, &ToolListEntry::onApplyClicked);
}

void
ToolListEntry::setTool(Tool* tool)
{
  m_layout.insertWidget(1, tool);

  m_tool = tool;

  m_tool->hide();
}

void
ToolListEntry::onApplyClicked()
{
  if (m_tool)
    emit toolApplied(*m_tool);
}

void
ToolListEntry::toggleToolVisibility()
{
  assert(m_tool);

  if (m_tool->isVisible()) {
    m_tool->hide();
    m_apply_button.hide();
  } else {
    m_tool->show();
    m_apply_button.show();
  }
}
