#pragma once

#include <QMatrix4x4>
#include <QObject>

class QMouseEvent;
class QWheelEvent;

class Camera : public QObject
{
public:
  Camera(QObject* parent);

  virtual ~Camera() = default;

  virtual void gotFocus() = 0;

  virtual void lostFocus() = 0;

  virtual void mouseMoveEvent(QMouseEvent*) = 0;

  virtual void wheelEvent(QWheelEvent*) = 0;

  virtual QMatrix4x4 viewMatrix() const = 0;
};
