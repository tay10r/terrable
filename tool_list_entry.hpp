#pragma once

#include <QFrame>
#include <QPushButton>
#include <QVBoxLayout>

class Tool;
class QString;

class ToolListEntryHeader : public QPushButton
{
  Q_OBJECT
public:
  ToolListEntryHeader(const QString& name, QWidget* parent);
};

class ToolListEntry final : public QFrame
{
  Q_OBJECT
public:
  ToolListEntry(QWidget* parent, const QString& name);

  void setTool(Tool* tool);

signals:
  void toolApplied(Tool& tool);

private:
  void onApplyClicked();

  void toggleToolVisibility();

private:
  QVBoxLayout m_layout{ this };

  ToolListEntryHeader m_label;

  QPushButton m_apply_button{ QObject::tr("Apply"), this };

  Tool* m_tool = nullptr;
};
