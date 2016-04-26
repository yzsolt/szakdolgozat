
#include "GUI.h"
#include <iostream>
using namespace nanogui;

glm::vec3 GUI::eigen4f_to_vec3(const Eigen::Vector4f& value) {
	return glm::vec3(value[0], value[1], value[2]);
}

Eigen::Vector4f GUI::vec3_to_eigen4f(const glm::vec3& value) {
	return Eigen::Vector4f(value.x, value.y, value.z, 1.0f);
}

GUI::GUI(::Window* main_window) : m_window(main_window) {

	initialize(main_window->glfw_window(), false);
	/*
	setCursorPosCallback([](double x, double y) {
		std::cout << x << ":" << y << std::endl;
	});

	setMouseButtonCallback([](int button, int action, int modifiers) {
		std::cout << button << " - " << action << " - " << modifiers << std::endl;
	});*/

	performLayout(mNVGContext);

}
