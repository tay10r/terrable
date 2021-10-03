#pragma once

#include <QFrame>

class Terrain;

class Tool : public QFrame
{
  Q_OBJECT
public:
  Tool(QWidget* parent);

  virtual ~Tool() = default;

  virtual bool execute(Terrain& terrain) = 0;
};
