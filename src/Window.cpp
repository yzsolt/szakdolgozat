
#include <stdexcept>

#include <glad/glad.h>

#include "Window.h"


void Window::_glfw_window_close_callback(GLFWwindow* glfw_window) {

	Window* window = static_cast<Window*>(glfwGetWindowUserPointer(glfw_window));
		
	bool close = true;

	if (window->m_close_callback) {
		close = window->m_close_callback();
	}

	glfwSetWindowShouldClose(glfw_window, close);

}

void Window::_glfw_cursor_position_callback(GLFWwindow* glfw_window, double x, double y) {

	Window* window = static_cast<Window*>(glfwGetWindowUserPointer(glfw_window));

	if (window->m_cursor_position_callback) {
		int height;
		glfwGetWindowSize(glfw_window, nullptr, &height);
		window->m_cursor_position_callback(glm::dvec2(x, static_cast<double>(height) - y));
	}

}

void Window::_glfw_key_callback(GLFWwindow* glfw_window, int key, int scancode, int action, int modifiers) {

	Window* window = static_cast<Window*>(glfwGetWindowUserPointer(glfw_window));

	if (window->m_key_callback) {
		window->m_key_callback(key, scancode, action, modifiers);
	}

}

Window::Window(const glm::uvec2& size, const std::string &title, GLuint multisampling, bool is_fullscreen, bool is_resizable, bool is_debug) {

    if (!glfwInit()) {
        throw std::runtime_error("Couldn't initialize GLFW.");
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    glfwWindowHint(GLFW_DEPTH_BITS, 24);

    glfwWindowHint(GLFW_SRGB_CAPABLE, true);

	if (!is_resizable) {
		glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	}

    if (is_debug) {
        // We check if debug context is available after we have the context
        glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
    }

    if (multisampling > 0) {
        // TODO: check if MSAA value is power of 2
        // If the given MSAA level is not supported, this will probably silently fail, we check it after we have the OpenGL context
        glfwWindowHint(GLFW_SAMPLES, multisampling);
    }

    GLFWmonitor* monitor = nullptr;
    int width = size.x;
    int height = size.y;

    if (is_fullscreen) {

        monitor = glfwGetPrimaryMonitor();

        const GLFWvidmode* mode = glfwGetVideoMode(monitor);

        glfwWindowHint(GLFW_RED_BITS, mode->redBits);
        glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
        glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
        glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);

        width = mode->width;
        height = mode->height;

    }

    m_window = glfwCreateWindow(
        width,
        height,
        title.c_str(),
        monitor,
        nullptr
    );

    if (!m_window) {
        throw std::runtime_error("Couldn't create GLFW window.");
    }

	glfwSetWindowUserPointer(m_window, this);

	glfwSetWindowCloseCallback(m_window, &Window::_glfw_window_close_callback);
	//glfwSetCursorPosCallback(m_window, &Window::_glfw_cursor_position_callback);
	//glfwSetKeyCallback(m_window, &Window::_glfw_key_callback);

	// Initialize GLAD

	make_context_current();

	int result = gladLoadGL();

	if (!result) {
		throw std::runtime_error("Couldn't initialize GLAD.");
	}

}

Window::~Window() {
    glfwDestroyWindow(m_window);
    glfwTerminate();
}

glm::uvec2 Window::size() const {
    glm::ivec2 size;
    glfwGetWindowSize(m_window, &size.x, &size.y);
    return size;
}

GLFWwindow* Window::glfw_window() const {
	return m_window;
}

void Window::poll_events() const {
	glfwPollEvents();
}

bool Window::vsync() const {
	return m_vsync;
}

void Window::set_vsync(bool vsync) {
	m_vsync = vsync;
	glfwSwapInterval(vsync ? 1 : 0);
}

void Window::set_close_callback(Window::CloseCallback callback) {
	m_close_callback = callback;
}

void Window::set_cursor_position_callback(Window::CursorPositionCallback callback) {
	m_cursor_position_callback = callback;
}

void Window::set_key_callback(Window::KeyCallback callback) {
	m_key_callback = callback;
}

void Window::make_context_current() {
    glfwMakeContextCurrent(m_window);
}

void Window::swap_buffers() {
    glfwSwapBuffers(m_window);
}

void Window::set_position(const glm::ivec2 &position) {
    glfwSetWindowPos(m_window, position.x, position.y);
}

void Window::centerize() {

    const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());

    glm::ivec2 screen_size(mode->width, mode->height);
    glm::ivec2 window_size;

    glfwGetWindowSize(m_window, &window_size.x, &window_size.y);

    screen_size /= 2;

    int left, top, right, bottom;
    glfwGetWindowFrameSize(m_window, &left, &top, &right, &bottom);

    window_size.x += left + right;
    window_size.y += top + bottom;

    window_size /= 2;

    set_position(screen_size - window_size);

}

bool Window::hide_cursor(bool hide) const {
	glfwSetInputMode(m_window, GLFW_CURSOR, hide ? GLFW_CURSOR_HIDDEN : GLFW_CURSOR_NORMAL);
	return glfwGetInputMode(m_window, GLFW_CURSOR) == GLFW_CURSOR_HIDDEN;
}

bool Window::lock_cursor(bool lock) const {
	glfwSetInputMode(m_window, GLFW_CURSOR, lock ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
	return glfwGetInputMode(m_window, GLFW_CURSOR) == GLFW_CURSOR_DISABLED;
}
