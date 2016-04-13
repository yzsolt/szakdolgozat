
#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera {

	glm::vec3 m_position;
	glm::vec3 m_direction;
	glm::vec2 m_yaw_pitch;
	glm::vec3 m_look_at = glm::vec3(0, 0, 0);
	glm::mat4 m_view_matrix;

	bool m_left_button_pressed = false;
	bool m_right_button_pressed = false;

	glm::vec2 m_radius_limit = glm::vec2(0.8f, 6.f);
	float m_radius = 1.5f;

	void _update();

public:

	Camera();

	void on_mouse_move(const glm::vec2& movement);

	void set_left_button_pressed(bool left_button_pressed);
	void set_right_button_pressed(bool right_button_pressed);

	const glm::vec3 position() const;

	void move(const glm::vec2& movement);

	const glm::mat4& view_matrix() const;

	float radius() const;

	void set_radius(float radius);

};

#endif
