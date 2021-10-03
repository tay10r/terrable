#pragma once

#include "camera.hpp"

#include <QTimer>
#include <QVector2D>
#include <QtMath>

class OrbitCamera final : public Camera
{
  Q_OBJECT
public:
  OrbitCamera(QObject* parent = nullptr);

  void gotFocus() override;

  void lostFocus() override;

  void mouseMoveEvent(QMouseEvent*) override;

  void wheelEvent(QWheelEvent*) override;

  QMatrix4x4 viewMatrix() const override;

signals:
  void cameraUpdate();

protected:
  void onTimeout();

private:
  struct FocusState
  {
    std::vector<QVector2D> mouse_movement_list;
  };

  float m_pitch = M_PI / 4;

  float m_yaw = M_PI / 4;

  float m_distance = 2;

  /// Indicates how much rotation occurs when the mouse moves.
  float m_radians_per_pixel = M_PI / 1024;

  /// Indicates how much zoom happens on mouse wheel events.
  float m_meters_per_radian = 0.01;

  QTimer m_move_timer;

  std::unique_ptr<FocusState> m_focus_state;
};
