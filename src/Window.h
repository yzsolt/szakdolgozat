
#ifndef WINDOW_H
#define WINDOW_H

#include <string>
#include <functional>
#include <memory>

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

class Window {

	GLFWwindow* m_window;

	bool m_vsync = true;

	typedef std::function<bool()> CloseCallback;
	typedef std::function<void(const glm::dvec2&)> CursorPositionCallback;
	typedef std::function<void(int key, int scancode, int action, int modifiers)> KeyCallback;

	CloseCallback m_close_callback;
	CursorPositionCallback m_cursor_position_callback;
	KeyCallback m_key_callback;

	static void _glfw_window_close_callback(GLFWwindow* glfw_window);
	static void _glfw_key_callback(GLFWwindow* window, int key, int scancode, int action, int modifiers);
	static void _glfw_cursor_position_callback(GLFWwindow* window, double x, double y);

public:

    Window(const glm::uvec2& size, const std::string& title, GLuint multisampling, bool is_fullscreen = false, bool is_debug = false);
    ~Window();

    glm::uvec2 size() const;
	GLFWwindow* glfw_window() const;

	void poll_events() const;

	bool vsync() const;
	void set_vsync(bool vsync);

	void set_close_callback(CloseCallback callback);
	void set_cursor_position_callback(CursorPositionCallback callback);
	void set_key_callback(KeyCallback callback);

    void make_context_current();
    void swap_buffers();

    void set_position(const glm::ivec2& position);
    void centerize();

	bool hide_cursor(bool hide) const;

	/** Locks/unlocks (also hides/shows) the cursor. Returns true on success, false otherwise. */
	bool lock_cursor(bool lock) const;

};

#endif
