
#ifndef GUI_H
#define GUI_H

#include <glad/glad.h>
#include <nanogui/nanogui.h>

#include "Window.h"

class GUI : public nanogui::Screen {

	::Window* m_window;

public:

	GUI(::Window* main_window);

	//bool mouseButtonCallbackEvent(int button, int action, int modifiers);

};

#endif
