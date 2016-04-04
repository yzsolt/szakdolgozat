
#include <iostream>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glad/glad.h>

#include "Renderer.h"
#include "Shader.h"
#include "Mesh.h"

using namespace nanogui;

float GLHalfToFloat(GLushort bits) {
	GLuint magic = 126 << 23;
	GLuint fp32 = (bits & 0x8000);
	GLuint mant = (bits & 0x000003ff);
	GLint exp = (bits >> 10) & 0x0000001f;

	if (exp == 0) {
		fp32 = magic + mant;
		(*(float*)&fp32) -= (*(float*)&magic);
	} else {
		mant <<= 13;

		if (exp == 31)
			exp = 255;
		else
			exp += 127 - 15;

		fp32 |= (exp << 23);
		fp32 |= mant;
	}

	return *((float*)&fp32);
}

void APIENTRY Renderer::_on_gl_message(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, const void *user_data) {

	(void)length;
	(void)user_data;

	/*
	switch (id)
	{
	case 131169: // The driver allocated storage for renderbuffer
	case 131185: // glBufferData
	return;
	}*/

	std::stringstream s;

	s << "OpenGL debug message";
	s << std::endl << " - from:     ";

	switch (source) {
	case GL_DEBUG_SOURCE_API:
		s << "API";
		break;
	case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
		s << "Window System";
		break;
	case GL_DEBUG_SOURCE_SHADER_COMPILER:
		s << "Shader Compiler";
		break;
	case GL_DEBUG_SOURCE_THIRD_PARTY:
		s << "Third Party";
		break;
	case GL_DEBUG_SOURCE_APPLICATION:
		s << "Application";
		break;
	case GL_DEBUG_SOURCE_OTHER:
		s << "Other";
		break;
	}

	s << std::endl << " - type:     ";

	switch (type) {
	case GL_DEBUG_TYPE_ERROR:
		s << "Error";
		break;
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
		s << "Deprecated Behaviour";
		break;
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
		s << "Undefined Behaviour";
		break;
	case GL_DEBUG_TYPE_PORTABILITY:
		s << "Portability";
		break;
	case GL_DEBUG_TYPE_PERFORMANCE:
		s << "Performance";
		break;
	case GL_DEBUG_TYPE_MARKER:
		s << "Marker";
		break;
	case GL_DEBUG_TYPE_PUSH_GROUP:
		s << "Push Group";
		break;
	case GL_DEBUG_TYPE_POP_GROUP:
		s << "Pop Group";
		break;
	case GL_DEBUG_TYPE_OTHER:
		s << "Other";
		break;
	}

	s << std::endl << " - ID:       " << id;

	s << " - severity: ";

	switch (severity) {
	case GL_DEBUG_SEVERITY_HIGH:
		s << "High";
		break;
	case GL_DEBUG_SEVERITY_MEDIUM:
		s << "Medium";
		break;
	case GL_DEBUG_SEVERITY_LOW:
		s << "Low";
		break;
	case GL_DEBUG_SEVERITY_NOTIFICATION:
		s << "Notification";
		break;
	}

	s << std::endl << " - message:  " << message;

	std::cout << "OpenGL debug message: " << s.str() << std::endl;

}

void Renderer::_measure_average_luminance() {

	glm::vec2 texel_size = glm::vec2(1) / glm::vec2(m_window.size());

	m_average_luminance_fb->bind();
	m_average_luminance_program->bind();

		// 64x64

		m_average_luminance_program->set_texture("u_luminance_texture", m_main_fb->color_texture(0));

		m_average_luminance_program->set_uniform("u_texel_size", texel_size);
		m_average_luminance_program->set_uniform("u_step", 0);
		m_fullscreen_quad.draw();

		m_average_luminance_program->set_texture("u_luminance_texture", m_average_luminance_fb->color_texture(0));

		// 16x16
		
		m_average_luminance_fb->set_color_texture_level(0, 2);

		m_average_luminance_program->set_uniform("u_previous_level", 0);
		m_average_luminance_program->set_uniform("u_step", 1);
		m_fullscreen_quad.draw();

		// 4x4

		m_average_luminance_fb->set_color_texture_level(0, 4);

		m_average_luminance_program->set_uniform("u_previous_level", 2);
		m_average_luminance_program->set_uniform("u_step", 1);
		m_fullscreen_quad.draw();

		// 1x1

		m_average_luminance_fb->set_color_texture_level(0, 6);

		m_average_luminance_program->set_uniform("u_previous_level", 4);
		m_average_luminance_program->set_uniform("u_step", 2);
		m_fullscreen_quad.draw();

		m_average_luminance_fb->set_color_texture_level(0, 0);
		
	m_average_luminance_program->unbind();
	m_average_luminance_fb->unbind();

}

void Renderer::_adapt_luminance() {

	m_previous_adapted_luminance_fb.swap(m_current_adapted_luminance_fb);

	m_current_adapted_luminance_fb->bind();
	m_luminance_adapter_program->bind();

		m_luminance_adapter_program->set_texture("u_previous_adapted_luminance_texture", m_previous_adapted_luminance_fb->color_texture(0));
		m_luminance_adapter_program->set_texture("u_average_luminance_texture", m_average_luminance_fb->color_texture(0));

		double current_time = glfwGetTime();
		m_luminance_adapter_program->set_uniform("u_elapsed_time", static_cast<float>(current_time - m_last_frame_time));
		m_last_frame_time = current_time;

		m_fullscreen_quad.draw();

		#if 0 //def _DEBUG
		GLushort bits;

		glReadPixels(0, 0, 1, 1, GL_RED, GL_HALF_FLOAT, &bits);

		float d = GLHalfToFloat(bits);
		std::cout << d << std::endl;
		//DEBUG_exposure = 1.0f / (9.6f * GLHalfToFloat(bits));

		//std::cout << "avg lum: " << DEBUG_avglum << ", exposure: " << DEBUG_exposure << "\n";
		#endif
	
	m_luminance_adapter_program->unbind();
	m_current_adapted_luminance_fb->unbind();

}

void Renderer::_setup_gui() {

	m_gui = std::make_unique<GUI>(&m_window);

	auto renderer_settings = new nanogui::Window(m_gui.get(), "Renderer settings");
	renderer_settings->setPosition(Eigen::Vector2i(15, 15));
	renderer_settings->setLayout(new GroupLayout());

	auto vsync_checkbox = new CheckBox(renderer_settings, "V-Sync", [this](bool state) {
		m_window.set_vsync(state);
	});
	vsync_checkbox->setChecked(m_window.vsync());

	new Label(renderer_settings, "Visualize");
	auto visualize = new ComboBox(renderer_settings, {
		"Nothing", "Positions", "Normals", "Tangents", "Ambient", "Diffuse", "Normal map", "Specularity", "Reflectivity"
	});
	visualize->setCallback([this](int index) {
		m_visualize = static_cast<Visualize>(index);
	});

	new Label(renderer_settings, "Tone mapping");
	auto tone_map = new ComboBox(renderer_settings, {
		"Reinhard", "Uncharted 2", "Off"
	});
	tone_map->setSelectedIndex(static_cast<int>(m_tone_map));
	tone_map->setCallback([this](int index) {
		m_tone_map = static_cast<ToneMap>(index);
	});

	new Label(renderer_settings, "Mesh settings");

	auto rotate_mesh_checkbox = new CheckBox(renderer_settings, "Rotate mesh", [this](bool state) {
		m_rotate_mesh = state;
	});
	rotate_mesh_checkbox->setChecked(m_rotate_mesh);

	auto load_mesh = new Button(renderer_settings, "Load mesh");
	load_mesh->setCallback([&] {
		std::string mesh_path = file_dialog({ { "obj", "Wavefront OBJ" } }, false);
		m_mesh.reset(new Mesh(mesh_path, m_gui.get()));
		m_mesh->upload();
	});

}

Renderer::Renderer(const Settings& settings) : m_window(settings.window_size, "PBR renderer", settings.multisample_count, settings.fullscreen, true) {

	m_window.set_close_callback([this]() -> bool {
		m_is_running = false;
		return true; // Let GLFW close the window
	});

	m_window.set_vsync(settings.vsync);

	// Debug

	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageCallback(_on_gl_message, nullptr);
	glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);

	_setup_gui();

	// Bind camera cursor callbacks

	m_gui->setCursorPosCallback([this](double x, double y) {
	
		glm::vec2 new_position = glm::vec2(x, -y);
		glm::vec2 movement = new_position - m_last_cursor_position;

		m_camera.on_mouse_move(movement);

		m_last_cursor_position = new_position;

	});

	m_gui->setMouseButtonCallback([this](int button, int action, int modifiers) {
		
		switch (button) {
		case GLFW_MOUSE_BUTTON_1:
			m_camera.set_mouse_control(action == GLFW_PRESS);
			break;
		}

	});

	m_gui->setScrollCallback([this](double x, double y) {

		m_camera.set_radius(m_camera.radius() - static_cast<float>(y));

	});

	//

	m_world = glm::mat4(1.f);

	m_projection = glm::perspectiveFov(89.f, static_cast<float>(settings.window_size.x), static_cast<float>(settings.window_size.y), 0.1f, 100.f);
	
	// Load programs

	m_fullscreen_program = std::make_unique<Program>("simple_2d.vs.glsl", "simple_2d.fs.glsl");

	m_average_luminance_program = std::make_unique<Program>("simple_2d.vs.glsl", "average_luminance.fs.glsl");
	m_luminance_adapter_program = std::make_unique<Program>("simple_2d.vs.glsl", "luminance_adapter.fs.glsl");
	m_tone_map_program = std::make_unique<Program>("simple_2d.vs.glsl", "tone_map.fs.glsl");

	m_blinn_phong_program = std::make_unique<Program>("pass_through.vs.glsl", "blinn_phong.fs.glsl");

	m_pbr_light_probe_program = std::make_unique<Program>("pbr_light_probe.vs.glsl", "image_based_lighting.fs.glsl");
	m_physically_based_program = std::make_unique<Program>("pass_through.vs.glsl", "physically_based.fs.glsl");

	// Create frame buffers

	m_main_fb = std::make_unique<FrameBuffer>(m_window.size());
	m_main_fb->bind();
	m_main_fb->attach_color_texture(Texture::InternalFormat::RGBA_16_FLOAT);
	m_main_fb->attach_depth_texture();
	m_main_fb->validate();

	m_average_luminance_fb = std::make_unique<FrameBuffer>(glm::uvec2(64, 64));
	m_average_luminance_fb->bind();
	size_t texture_id = m_average_luminance_fb->attach_color_texture(Texture::InternalFormat::R_16_FLOAT);
	m_average_luminance_fb->validate();
	m_average_luminance_fb->color_texture(texture_id).generate_mipmap();

	m_previous_adapted_luminance_fb = std::make_unique<FrameBuffer>(glm::uvec2(1, 1));
	m_previous_adapted_luminance_fb->bind();
	m_previous_adapted_luminance_fb->attach_color_texture(Texture::InternalFormat::R_16_FLOAT);
	m_previous_adapted_luminance_fb->validate();

	m_current_adapted_luminance_fb = std::make_unique<FrameBuffer>(glm::uvec2(1, 1));
	m_current_adapted_luminance_fb->bind();
	m_current_adapted_luminance_fb->attach_color_texture(Texture::InternalFormat::R_16_FLOAT);
	m_current_adapted_luminance_fb->validate();
	glClearColor(0.5f, 0.4f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	//	m_current_adapted_luminance_fb->unbind();

	// Load meshes
	
	m_mesh = std::make_unique<Mesh>("data/handgun/Handgun_obj.obj", m_gui.get());
	//m_mesh = std::make_unique<Mesh>("data/watertank/Water_Tank_BI.obj", m_gui.get());
	//m_mesh = std::make_unique<Mesh>("data/nanosuit/nanosuit.obj", m_gui.get());
	//m_mesh = std::make_unique<Mesh>("data/ferrari_599gtb.obj");
	m_mesh->upload();

	// Create skybox

	m_skybox = std::make_unique<Skybox>("grace");

	m_last_frame_time = glfwGetTime();

	//
	
	glEnable(GL_FRAMEBUFFER_SRGB);
	glEnable(GL_DEPTH_TEST);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

}

void Renderer::run() {

	while (m_is_running) {

		m_window.poll_events();

		//glm::uvec2 window_size = m_window.size();
		//glViewport(0, 0, window_size.x, window_size.y);

		m_main_fb->bind();

			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
			glClearColor(0.5f, 0.5f, 0.5f, 1.0f);

			if (m_rotate_mesh) {
				m_mesh_rotation += static_cast<float>(glfwGetTime() - m_last_frame_time) / 2.f;
			}

			// Matrix calculations

			m_world = glm::mat4(1);
			m_world = glm::rotate(m_world, m_mesh_rotation, glm::vec3(0, 1, 0));

			m_world = glm::translate(m_world, -m_mesh->center());

			glm::mat4 view = m_camera.view_matrix();

			m_world_view = view * m_world;
			m_normal_matrix = glm::transpose(glm::inverse(m_world_view));

			// Draw mesh

			Program& program = m_mesh->use_pbr() ? *m_pbr_light_probe_program : *m_blinn_phong_program;

			program.bind();

			program.set_uniform("u_world", m_world);
			//program.set_uniform("u_view", m_view);
			program.set_uniform("u_projection", m_projection);
			program.set_uniform("u_world_view", m_world_view);
			//program.set_uniform("u_normal_matrix", m_normal_matrix);
			program.set_uniform("u_view_position", m_camera.position());
			program.set_uniform("u_visualize", static_cast<int>(m_visualize));
			program.set_texture("u_environment_map", m_skybox->environment_map());

			if (m_mesh->use_pbr()) {

				program.set_uniform("u_world_inverse", glm::inverse(m_world));
				program.set_uniform("u_view_projection", m_projection * view);

				program.set_texture("u_diffuse_irradiance_map", m_skybox->diffuse_irradiance_map());
				program.set_texture("u_specular_irradiance_map", m_skybox->specular_irradiance_map());
				program.set_texture("u_brdf_lut", m_skybox->brdf_lut());

			}

			m_mesh->draw(program);

			// Draw skybox

			glm::mat4 view_without_translation = glm::mat4(glm::mat3(view));
			glDepthFunc(GL_LEQUAL);
			m_skybox->draw(m_projection * view_without_translation);
			glDepthFunc(GL_LESS);

		m_main_fb->unbind();

		// Measure average luminance, then adapt

		_measure_average_luminance();
		_adapt_luminance();

		// Tone mapping

		glm::uvec2 screen_size = m_window.size();
		glViewport(0, 0, screen_size.x, screen_size.y);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		glClearColor(0.5f, 0.5f, 0.5f, 1.0f);

		if (m_tone_map != ToneMap::OFF) {

			m_tone_map_program->bind();
				
				m_tone_map_program->set_texture("u_hdr_texture", m_main_fb->color_texture(0));
				m_tone_map_program->set_uniform("u_tone_map", static_cast<GLint>(m_tone_map));
				m_tone_map_program->set_texture("u_average_luminance_texture", m_current_adapted_luminance_fb->color_texture(0));

				m_fullscreen_quad.draw();

			m_tone_map_program->unbind();

		} else {

			m_fullscreen_program->bind();
				m_fullscreen_program->set_texture("u_texture", m_main_fb->color_texture(0));
				m_fullscreen_quad.draw();
			m_fullscreen_program->unbind();

		}

		// Draw GUI

		m_gui->drawAll();
		glEnable(GL_DEPTH_TEST);

		m_window.swap_buffers();

	}

}