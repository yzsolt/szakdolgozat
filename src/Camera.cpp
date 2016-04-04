
#include "Camera.h"

void Camera::_update() {

	m_direction = glm::normalize(glm::vec3(
		std::cos(m_yaw_pitch.y) * std::sin(m_yaw_pitch.x),
		std::sin(m_yaw_pitch.y),
		std::cos(m_yaw_pitch.y) * std::cos(m_yaw_pitch.x)
	));

	//m_view_matrix = glm::lookAt(m_position, m_position + direction, glm::vec3(0, 1, 0));
	m_position = -m_direction * m_radius;
	m_view_matrix = glm::lookAt(m_position, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));

}

void Camera::on_mouse_move(const glm::vec2& movement) {

	if (!m_mouse_control_enabled) {
		return;
	}

	m_yaw_pitch += movement * 0.01f;

	_update();

}

void Camera::set_mouse_control(bool mouse_control) {
	m_mouse_control_enabled = mouse_control;
}

const glm::vec3 Camera::position() const {
	return m_position;
}

const glm::mat4& Camera::view_matrix() const {
	return m_view_matrix;
}

float Camera::radius() const {
	return m_radius;
}

void Camera::set_radius(float radius) {
	m_radius = radius;
	_update();
}