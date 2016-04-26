
#ifndef GUI_H
#define GUI_H

#include <glad/glad.h>
#include <nanogui/nanogui.h>

#include "Window.h"

class GUI : public nanogui::Screen {

	::Window* m_window;

public:

	static glm::vec3 eigen4f_to_vec3(const Eigen::Vector4f& value);
	static Eigen::Vector4f vec3_to_eigen4f(const glm::vec3& value);

	GUI(::Window* main_window);

	//bool mouseButtonCallbackEvent(int button, int action, int modifiers);

};

#endif
