#pragma once

#include <QFrame>
#include <QPushButton>
#include <QVBoxLayout>

class Tool;
class QString;

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

  QPushButton m_label{ this };

  QPushButton m_apply_button{ QObject::tr("Apply"), this };

  Tool* m_tool = nullptr;
};
