#include "orbit_camera.hpp"

#include <QMouseEvent>

OrbitCamera::OrbitCamera(QObject* parent)
  : Camera(parent)
  , m_move_timer(this)
{
  m_move_timer.setInterval(50);

  connect(&m_move_timer, &QTimer::timeout, this, &OrbitCamera::onTimeout);
}

void
OrbitCamera::gotFocus()
{
  m_focus_state.reset(new FocusState());

  m_move_timer.start();
}

void
OrbitCamera::lostFocus()
{
  m_focus_state.reset();

  m_move_timer.stop();
}

void
OrbitCamera::wheelEvent(QWheelEvent* event)
{
  if (!m_focus_state)
    return;

  const float angle_delta = qDegreesToRadians(event->angleDelta().y() * 8.0f);

  m_distance -= angle_delta * m_meters_per_radian;

  emit cameraUpdate();
}

void
OrbitCamera::mouseMoveEvent(QMouseEvent* event)
{
  if (!m_focus_state)
    return;

  QVector2D mouse_position(event->x(), event->y());

  if (m_focus_state->mouse_movement_list.size() < 2)
    m_focus_state->mouse_movement_list.emplace_back(mouse_position);
  else
    m_focus_state->mouse_movement_list[1] = mouse_position;
}

void
OrbitCamera::onTimeout()
{
  if (!m_focus_state || (m_focus_state->mouse_movement_list.size() < 2))
    return;

  const QVector2D p0 = m_focus_state->mouse_movement_list[0];
  const QVector2D p1 = m_focus_state->mouse_movement_list[1];

  const int delta_x = p1.x() - p0.x();
  const int delta_y = p1.y() - p0.y();

  m_yaw += delta_x * m_radians_per_pixel;

  m_pitch += delta_y * m_radians_per_pixel;

  m_focus_state->mouse_movement_list.erase(m_focus_state->mouse_movement_list.begin());

  emit cameraUpdate();
}

QMatrix4x4
OrbitCamera::viewMatrix() const
{
  // Note: this is in a Z+ up coordinate system.
  const float dir_x = m_distance * std::cos(m_yaw) * std::sin(m_pitch);
  const float dir_y = m_distance * std::sin(m_yaw) * std::sin(m_pitch);
  const float dir_z = m_distance * std::cos(m_pitch);

  const QVector3D eye(dir_y, dir_z, -dir_x);

  QMatrix4x4 result;

  result.lookAt(eye, QVector3D(0, 0, 0), QVector3D(0, 1, 0));

  return result;
}
