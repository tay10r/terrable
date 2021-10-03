#pragma once

#include <QLabel>
#include <QScrollArea>
#include <QVBoxLayout>

#include <vector>

class Tool;
class ToolListEntry;
class QString;

class ToolList final : public QWidget
{
  Q_OBJECT
public:
  ToolList(QWidget* parent);

signals:
  void toolApplied(Tool&);

private:
  void addTool(const QString& name, Tool* tool);

private:
  QVBoxLayout m_root_layout{ this };

  QLabel m_list_header{ QObject::tr("Tools"), this };

  QScrollArea m_list{ this };

  QVBoxLayout m_list_layout{ &m_list };

  std::vector<ToolListEntry*> m_tools;
};
