
#include "GUI.h"
#include <iostream>
using namespace nanogui;

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
