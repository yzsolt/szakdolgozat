
#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera {
	glm::vec3 m_position;
	glm::vec3 m_direction;
	glm::vec2 m_yaw_pitch;
	//glm::vec3 m_direction;
	glm::mat4 m_view_matrix;
	bool m_mouse_control_enabled = false;
	float m_radius = 10.f;

	void _update();

public:

	void on_mouse_move(const glm::vec2& movement);

	void set_mouse_control(bool mouse_control);

	const glm::vec3 position() const;

	const glm::mat4& view_matrix() const;

	float radius() const;

	void set_radius(float radius);

};

#endif
