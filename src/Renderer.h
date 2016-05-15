
#ifndef RENDERER_H
#define RENDERER_H

#include <memory>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Camera.h"
#include "FrameBuffer.h"
#include "FullscreenQuad.h"
#include "GUI.h"
#include "lights.h"
#include "Mesh.h"
#include "Program.h"
#include "Settings.h"
#include "Skybox.h"
#include "Window.h"

class Renderer {

public:

	enum class ToneMap : int {
		REINHARD	= 0,
		UNCHARTED_2	= 1,
		UNREAL_4	= 2,
		OFF			= 3,
	};

private:

	static void APIENTRY _on_gl_message(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, const void *user_data);

	Settings m_settings;

	::Window m_window;
	bool m_is_running = true;
	std::unique_ptr<GUI> m_gui;

	FullscreenQuad m_fullscreen_quad;
	std::unique_ptr<FrameBuffer> m_main_fb;
	std::unique_ptr<FrameBuffer> m_msaa_fb;

	// Tone mapping

	float m_exposure = -1.f;
	nanogui::Slider* m_exposure_slider = nullptr;

	ToneMap m_tone_map = ToneMap::UNCHARTED_2;

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

	// The mesh

	std::unique_ptr<Mesh> m_mesh;
	bool m_rotate_mesh = true;
	float m_mesh_rotation = 0.f;

	bool m_use_ibl = true;

	// Programs

	std::unique_ptr<Program> m_fullscreen_program;

	std::unique_ptr<Program> m_average_luminance_program;
	std::unique_ptr<Program> m_luminance_adapter_program;
	std::unique_ptr<Program> m_tone_map_program;

	std::unique_ptr<Program> m_basic_color_program;

	std::unique_ptr<Program> m_bp_image_based_lighting_program;
	std::unique_ptr<Program> m_bp_direct_lighting_program;

	std::unique_ptr<Program> m_pb_image_based_lighting_program;
	std::unique_ptr<Program> m_pb_direct_lighting_program;

	std::vector<std::unique_ptr<Light>> m_lights;

	void _measure_average_luminance();
	void _adapt_luminance();

	void _setup_gui();

public:

	Renderer(const Settings& settings);

	void run();

};

#endif
