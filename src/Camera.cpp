
#include "log.h"
#include <string>
#include "Camera.h"

void Camera::_update() {

	m_direction = glm::normalize(glm::vec3(
		std::cos(m_yaw_pitch.y) * std::sin(m_yaw_pitch.x),
		std::sin(m_yaw_pitch.y),
		std::cos(m_yaw_pitch.y) * std::cos(m_yaw_pitch.x)
	));

	//m_view_matrix = glm::lookAt(m_position, m_position + direction, glm::vec3(0, 1, 0));
	m_position = -m_direction * m_radius;
	m_view_matrix = glm::lookAt(m_position, m_look_at, glm::vec3(0, 1, 0));

}

Camera::Camera() {
	_update();
}

void Camera::on_mouse_move(const glm::vec2& movement) {

	if (!m_left_button_pressed && !m_right_button_pressed) {
		return;
	}

	if (m_left_button_pressed) {
		m_yaw_pitch += movement * 0.01f;
		m_yaw_pitch.y = glm::clamp(m_yaw_pitch.y, -glm::half_pi<float>() + 0.001f, glm::half_pi<float>() - 0.001f);
	}

	if (m_right_button_pressed) {
		move(movement);
	}

	_update();

}

void Camera::set_left_button_pressed(bool left_button_pressed) {
	m_left_button_pressed = left_button_pressed;
}

void Camera::set_right_button_pressed(bool right_button_pressed) {
	m_right_button_pressed = right_button_pressed;
}

const glm::vec3 Camera::position() const {
	return m_position;
}

void Camera::move(const glm::vec2& movement) {
	/*
	glm::vec4 right = glm::vec4(glm::normalize(glm::cross(m_direction, glm::vec3(0, 1, 0))), 1);

	float angle = 0.f;
	if (movement.y != 0.f) {
		angle = std::tan(movement.x / movement.y);

		if (movement.x < 0) {
			angle += glm::pi<float>();
		}
	}

	glm::mat4 rot = glm::rotate(glm::mat4(1), angle, m_direction);

	right = right * rot;
	right *= movement.length() / 10.f;

	glm::vec3 movement_3d = glm::vec3(right.x, right.y, right.z);

	m_look_at += movement_3d;
	m_position += movement_3d;
	*/
}

const glm::mat4& Camera::view_matrix() const {
	return m_view_matrix;
}

float Camera::radius() const {
	return m_radius;
}

void Camera::set_radius(float radius) {

	m_radius = glm::clamp(radius, m_radius_limit.x, m_radius_limit.y);
	
	_update();

}