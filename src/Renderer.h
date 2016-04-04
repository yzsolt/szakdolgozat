
#ifndef RENDERER_H
#define RENDERER_H

#include <memory>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Camera.h"
#include "FrameBuffer.h"
#include "FullscreenQuad.h"
#include "GUI.h"
#include "Mesh.h"
#include "Program.h"
#include "Settings.h"
#include "Skybox.h"
#include "Window.h"

class Renderer {

public:

	enum class Visualize : int {
		NOTHING			= 0,
		POSITIONS		= 1,
		NORMALS			= 2,
		TANGENTS		= 3,
		AMBIENT			= 4,
		DIFFUSE			= 5,
		NORMAL_MAP		= 6,
		SPECULARITY		= 7,
		REFLECTIVITY	= 8
	};

	enum class ToneMap : int {
		REINHARD	= 0,
		UNCHARTED_2	= 1,
		OFF			= 2
	};

private:

	static void APIENTRY _on_gl_message(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, const void *user_data);

	::Window m_window;
	bool m_is_running = true;
	std::unique_ptr<GUI> m_gui;

	FullscreenQuad m_fullscreen_quad;
	std::unique_ptr<FrameBuffer> m_main_fb;
	std::unique_ptr<FrameBuffer> m_average_luminance_fb;
	std::unique_ptr<FrameBuffer> m_previous_adapted_luminance_fb;
	std::unique_ptr<FrameBuffer> m_current_adapted_luminance_fb;

	Camera m_camera;
	glm::vec2 m_last_cursor_position;
	double m_last_frame_time;

	glm::mat4 m_world;
	glm::mat4 m_projection;

	glm::mat4 m_world_view;
	glm::mat4 m_normal_matrix;

	std::unique_ptr<Skybox> m_skybox;
	std::unique_ptr<Mesh> m_mesh;
	bool m_rotate_mesh = true;

	Visualize m_visualize = Visualize::NOTHING;
	ToneMap m_tone_map = ToneMap::UNCHARTED_2;

	std::unique_ptr<Program> m_fullscreen_program;

	std::unique_ptr<Program> m_average_luminance_program;
	std::unique_ptr<Program> m_luminance_adapter_program;
	std::unique_ptr<Program> m_tone_map_program;

	std::unique_ptr<Program> m_blinn_phong_program;

	std::unique_ptr<Program> m_pbr_light_probe_program;
	std::unique_ptr<Program> m_physically_based_program;

	void _measure_average_luminance();
	void _adapt_luminance();

public:

	Renderer(const Settings& settings);

	void run();

};

#endif
