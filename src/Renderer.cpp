
#include <iostream>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glad/glad.h>

#include "log.h"
#include "Renderer.h"
#include "Shader.h"
#include "Mesh.h"

using namespace nanogui;

void APIENTRY Renderer::_on_gl_message(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, const void *user_data) {

	(void)length;
	(void)user_data;

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

	m_average_luminance_fb->bind_for_drawing();
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

	m_current_adapted_luminance_fb->bind_for_drawing();
	m_luminance_adapter_program->bind();

		m_luminance_adapter_program->set_texture("u_previous_adapted_luminance_texture", m_previous_adapted_luminance_fb->color_texture(0));
		m_luminance_adapter_program->set_texture("u_average_luminance_texture", m_average_luminance_fb->color_texture(0));

		double current_time = glfwGetTime();
		m_luminance_adapter_program->set_uniform("u_elapsed_time", static_cast<float>(current_time - m_last_frame_time));
		m_last_frame_time = current_time;

		m_fullscreen_quad.draw();

	m_luminance_adapter_program->unbind();
	m_current_adapted_luminance_fb->unbind();

}

void Renderer::_setup_gui() {

	m_gui = std::make_unique<GUI>(&m_window);

	// Renderer settings

	auto renderer_settings = new nanogui::Window(m_gui.get(), "Renderer settings");
	renderer_settings->setPosition(Eigen::Vector2i(5, 5));
	renderer_settings->setLayout(new GroupLayout());

	auto vsync_checkbox = new CheckBox(renderer_settings, "V-Sync", [this](bool state) {
		m_window.set_vsync(state);
	});
	vsync_checkbox->setChecked(m_window.vsync());

	new Label(renderer_settings, "Skybox");
	auto load_skybox = new Button(renderer_settings, "Load skybox");
	load_skybox->setCallback([&] {

		std::string skybox_path = file_dialog({ { "hdr", "Radiance HDR" } }, false);

		if (!skybox_path.empty()) {
			m_skybox->reset_panorama(skybox_path);
		}

	});

	new Label(renderer_settings, "Tone mapping");
	auto tone_map = new ComboBox(renderer_settings, {
		"Reinhard", "Uncharted 2", "Unreal 4", "Off"
	});
	tone_map->setSelectedIndex(static_cast<int>(m_tone_map));
	tone_map->setCallback([this](int index) {
		m_tone_map = static_cast<ToneMap>(index);
	});

	new Label(renderer_settings, "Exposure");
	auto auto_exposure_checkbox = new CheckBox(renderer_settings, "Automatic exposure", [this](bool state) {
		m_exposure *= -1;
		m_exposure_slider->setEnabled(!state);
	});
	auto_exposure_checkbox->setChecked(m_exposure < 0);

	const float exposure_multiplier = 16;

	m_exposure_slider = new Slider(renderer_settings);
	m_exposure_slider->setValue(m_exposure * -1 / exposure_multiplier);
	m_exposure_slider->setEnabled(m_exposure >= 0);
	m_exposure_slider->setCallback([this, exposure_multiplier](float value) {
		m_exposure = value * exposure_multiplier;
	});

	new Label(renderer_settings, "Mesh settings");
	auto rotate_mesh_checkbox = new CheckBox(renderer_settings, "Rotate mesh", [this](bool state) {
		m_rotate_mesh = state;
	});
	rotate_mesh_checkbox->setChecked(m_rotate_mesh);

	auto load_mesh = new Button(renderer_settings, "Load mesh");
	load_mesh->setCallback([&] {

		std::string mesh_path = file_dialog({ { "obj", "Wavefront OBJ" } }, false);

		if (!mesh_path.empty()) {
			m_mesh.reset(new Mesh(mesh_path, m_gui.get()));
			m_mesh->upload();
		}

	});

	// Lighting settings

	auto lighting_settings = new nanogui::Window(m_gui.get(), "Lighting settings");
	lighting_settings->setPosition(Eigen::Vector2i(400, 5));
	lighting_settings->setLayout(new GroupLayout());

	auto ibl_checkbox = new CheckBox(lighting_settings, "Use IBL", [this](bool state) {
		m_use_ibl = state;
	});
	ibl_checkbox->setChecked(m_use_ibl);

	auto grid_layout = new GridLayout(Orientation::Horizontal, 2, Alignment::Middle, 15, 5);
	grid_layout->setColAlignment({ Alignment::Maximum, Alignment::Fill });
	grid_layout->setSpacing(0, 10);

	new Label(lighting_settings, "Point light");

	PointLight* point_light = static_cast<PointLight*>(m_lights[0].get());

	auto point_light_grid = new Widget(lighting_settings);
	point_light_grid->setLayout(grid_layout);

	new Label(point_light_grid, "Use:", "sans-bold");
	auto use_point_light = new CheckBox(point_light_grid, "", [point_light](bool state) {
		point_light->set_on(state);
	});
	use_point_light->setChecked(point_light->is_on());

	new Label(point_light_grid, "Color:", "sans-bold");
	auto point_light_color = new ColorPicker(point_light_grid, GUI::vec3_to_eigen4f(point_light->color()));
	point_light_color->setCallback([point_light](const Color& color) {
		point_light->set_color(GUI::eigen4f_to_vec3(color));
	});
	point_light_color->setColor(GUI::vec3_to_eigen4f(point_light->color()));

	new Label(point_light_grid, "Luminous flux:", "sans-bold");
	auto point_light_flux = new Slider(point_light_grid);
	point_light_flux->setCallback([point_light](float value) {
		point_light->set_luminous_flux(value * 1000.f);
	});
	point_light_flux->setValue(point_light->luminous_flux() / 1000.f);

	new Label(lighting_settings, "Spot light");

	SpotLight* spot_light = static_cast<SpotLight*>(m_lights[1].get());

	auto spot_light_grid = new Widget(lighting_settings);
	spot_light_grid->setLayout(grid_layout);

	new Label(spot_light_grid, "Use:", "sans-bold");
	auto use_spot_light = new CheckBox(spot_light_grid, "", [spot_light](bool state) {
		spot_light->set_on(state);
	});
	use_spot_light->setChecked(spot_light->is_on());

	new Label(spot_light_grid, "Color:", "sans-bold");
	auto spot_light_color = new ColorPicker(spot_light_grid, GUI::vec3_to_eigen4f(spot_light->color()));
	spot_light_color->setCallback([spot_light](const Color& color) {
		spot_light->set_color(GUI::eigen4f_to_vec3(color));
	});
	spot_light_color->setColor(GUI::vec3_to_eigen4f(spot_light->color()));

	new Label(spot_light_grid, "Luminous flux:", "sans-bold");
	auto spot_light_flux = new Slider(spot_light_grid);
	spot_light_flux->setCallback([spot_light](float value) {
		spot_light->set_luminous_flux(value * 1000.f);
	});
	spot_light_flux->setValue(spot_light->luminous_flux() / 1000.f);

	const float D = glm::pi<float>() * 4;

	new Label(spot_light_grid, "Inner cone angle:", "sans-bold");
	auto spot_light_inner = new Slider(spot_light_grid);
	spot_light_inner->setCallback([spot_light, D](float value) {
		spot_light->set_inner_cone_angle(value * D);
	});
	spot_light_inner->setValue(spot_light->inner_cone_angle() / D);

	new Label(spot_light_grid, "Outer cone angle:", "sans-bold");
	auto spot_light_outer = new Slider(spot_light_grid);
	spot_light_outer->setCallback([spot_light, D](float value) {
		spot_light->set_outer_cone_angle(value * D);
	});
	spot_light_outer->setValue(spot_light->outer_cone_angle() / D);

}

Renderer::Renderer(const Settings& settings) : 
	m_settings(settings),
	m_window(settings.window_size, "PBR renderer", settings.multisample_count, settings.fullscreen, false, true) 
{

	m_window.set_close_callback([this]() -> bool {
		m_is_running = false;
		return true; // Let GLFW close the window
	});

	m_window.set_vsync(true);

	// Debug

	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageCallback(_on_gl_message, nullptr);
	glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);

	// Add lights

	m_lights.push_back(std::make_unique<PointLight>(
		// Position
		glm::vec3(0.8f, 0.8f, 0.8f),
		// Color
		glm::vec3(1, 0, 0),
		// Radius
		9.f,
		// Luminous flux
		600.f
	));

	m_lights.push_back(std::make_unique<SpotLight>(
		// Position
		m_camera.position(),
		// Color
		glm::vec3(0, 0, 1),
		// Radius
		9.f,
		// Luminous flux
		600.f,
		// Inner
		2.5f,
		// Outer
		3.f
	));

	// Setup GUI

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
		case GLFW_MOUSE_BUTTON_LEFT:
			m_camera.set_left_button_pressed(action == GLFW_PRESS);
			break;
		case GLFW_MOUSE_BUTTON_RIGHT:
			m_camera.set_right_button_pressed(action == GLFW_PRESS);
			break;
		}

	});

	m_gui->setScrollCallback([this](double x, double y) {

		y /= 10.f;

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

	m_basic_color_program = std::make_unique<Program>("basic_color.vs.glsl", "basic_color.fs.glsl");

	m_blinn_phong_program = std::make_unique<Program>("blinn_phong.vs.glsl", "blinn_phong.fs.glsl");

	m_image_based_lighting_program = std::make_unique<Program>("pbr_light_probe.vs.glsl", "image_based_lighting.fs.glsl");
	m_direct_lighting_program = std::make_unique<Program>("direct_lighting.vs.glsl", "direct_lighting.fs.glsl");

	// Create frame buffers

	m_main_fb = std::make_unique<FrameBuffer>(m_window.size());
	m_main_fb->bind();
		m_main_fb->attach_color_texture(Texture::InternalFormat::RGBA_16_FLOAT);
		m_main_fb->attach_depth_texture();
		m_main_fb->validate();
	m_main_fb->unbind();

	if (m_settings.multisample_count > 0) {

		GLint max_samples;
		glGetIntegerv(GL_MAX_SAMPLES, &max_samples);

		if ((m_settings.multisample_count & (m_settings.multisample_count - 1)) != 0) {
			LOG("Sample count for MSAA is not a power of two, disabling MSAA.");
		} else {
		
			if (m_settings.multisample_count > max_samples) {
				LOG("Implementation can't provide " + std::to_string(m_settings.multisample_count) + " samples, falling back to maximum (" + std::to_string(max_samples) + ").");
				m_settings.multisample_count = max_samples;
			}

			m_msaa_fb = std::make_unique<FrameBuffer>(m_window.size());
			m_msaa_fb->bind();
				m_msaa_fb->attach_render_target(FrameBuffer::AttachmentType::DEPTH, Texture::InternalFormat::DEPTH_32, m_settings.multisample_count);
				m_msaa_fb->attach_render_target(FrameBuffer::AttachmentType::COLOR, Texture::InternalFormat::RGBA_16_FLOAT, m_settings.multisample_count);
				m_msaa_fb->validate();
			m_msaa_fb->unbind();

			LOG("MSAA frame buffer created with " + std::to_string(m_settings.multisample_count) + " samples.");

		}

	}

	m_average_luminance_fb = std::make_unique<FrameBuffer>(glm::uvec2(64, 64));
	m_average_luminance_fb->bind();
		size_t texture_id = m_average_luminance_fb->attach_color_texture(Texture::InternalFormat::R_16_FLOAT);
		m_average_luminance_fb->validate();
		m_average_luminance_fb->color_texture(texture_id).generate_mipmap();
	m_average_luminance_fb->unbind();

	m_previous_adapted_luminance_fb = std::make_unique<FrameBuffer>(glm::uvec2(1, 1));
	m_previous_adapted_luminance_fb->bind();
		m_previous_adapted_luminance_fb->attach_color_texture(Texture::InternalFormat::R_16_FLOAT);
		m_previous_adapted_luminance_fb->validate();
	m_previous_adapted_luminance_fb->unbind();

	m_current_adapted_luminance_fb = std::make_unique<FrameBuffer>(glm::uvec2(1, 1));
	m_current_adapted_luminance_fb->bind();
		m_current_adapted_luminance_fb->attach_color_texture(Texture::InternalFormat::R_16_FLOAT);
		m_current_adapted_luminance_fb->validate();
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
	m_current_adapted_luminance_fb->unbind();

	// Load the default mesh
	
	m_mesh = std::make_unique<Mesh>("data/meshes/sphere.obj", m_gui.get());
	m_mesh->upload();

	// Load the default skybox

	m_skybox = std::make_unique<Skybox>(Skybox::ROOT_DIRECTORY + "at_the_window/at_the_window.hdr");

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

		glEnable(GL_DEPTH_TEST);
		glDisable(GL_CULL_FACE);

		FrameBuffer* render_target = m_msaa_fb ? m_msaa_fb.get() : m_main_fb.get();

		render_target->bind_for_drawing();

			if (m_rotate_mesh) {
				m_mesh_rotation += static_cast<float>(glfwGetTime() - m_last_frame_time) / 2.f;
			}

			// Matrix calculations

			m_world = glm::mat4(1);
			m_world = glm::scale(m_world, glm::vec3(m_mesh->scale()));
			m_world = glm::rotate(m_world, m_mesh_rotation, glm::vec3(0, 1, 0));
			m_world = glm::translate(m_world, -m_mesh->center());

			glm::mat4 view = m_camera.view_matrix();

			m_world_view = view * m_world;
			m_normal_matrix = glm::transpose(glm::inverse(m_world_view));

			glm::mat4 world_inverse = glm::inverse(m_world);
			glm::mat4 view_projection = m_projection * view;

			// Depth-only prepass

			glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

			glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

			glDisable(GL_BLEND);
			glDepthMask(GL_TRUE);

			m_basic_color_program->bind();

				m_basic_color_program->set_uniform("u_world", m_world);
				m_basic_color_program->set_uniform("u_view_projection", view_projection);

				m_mesh->draw();

			m_basic_color_program->unbind();

			glDepthMask(GL_FALSE);
			
			glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
			glDepthFunc(GL_EQUAL);

			// Draw mesh with image based lighting

			if (m_use_ibl) {

				Program& program = m_mesh->use_pbr() ? *m_image_based_lighting_program : *m_blinn_phong_program;

				program.bind();

					program.set_uniform("u_world", m_world);
					program.set_uniform("u_projection", m_projection);
					program.set_uniform("u_world_view", m_world_view);
					program.set_uniform("u_view_position", m_camera.position());
					//program.set_uniform("u_visualize", static_cast<int>(m_visualize));
					program.set_texture("u_environment_map", m_skybox->environment_map());

					if (m_mesh->use_pbr()) {

						program.set_uniform("u_world_inverse", world_inverse);
						program.set_uniform("u_view_projection", view_projection);

						program.set_texture("u_diffuse_irradiance_map", m_skybox->diffuse_irradiance_map());
						program.set_texture("u_specular_irradiance_map", m_skybox->specular_irradiance_map());
						program.set_texture("u_brdf_lut", m_skybox->brdf_lut());

					} else {

						// Lights

					}

					m_mesh->draw(&program);

				program.unbind();

			}

			glDepthMask(GL_FALSE);

			if (m_mesh->use_pbr()) {

				// Draw mesh with direct lights

				glEnable(GL_BLEND);
				glBlendFunc(GL_ONE, GL_ONE);

				m_direct_lighting_program->bind();

					m_direct_lighting_program->set_uniform("u_world", m_world);
					m_direct_lighting_program->set_uniform("u_world_inverse", world_inverse);
					m_direct_lighting_program->set_uniform("u_projection", m_projection);
					m_direct_lighting_program->set_uniform("u_view_projection", view_projection);
					m_direct_lighting_program->set_uniform("u_world_view", m_world_view);
					m_direct_lighting_program->set_uniform("u_view_position", m_camera.position());

					for (const auto& light : m_lights) {

						Light* light_ptr = light.get();

						if (light_ptr->is_on()) {

							switch (light->type()) {
							case Light::Type::POINT:
								static_cast<PointLight*>(light_ptr)->set_uniforms(*m_direct_lighting_program);
								break;
							case Light::Type::SPOT:
								SpotLight* spot_light = static_cast<SpotLight*>(light_ptr);
								spot_light->set_uniforms(*m_direct_lighting_program);
								spot_light->set_direction(m_camera.direction());
								spot_light->set_position(m_camera.position());
								break;
							}

							m_mesh->draw(m_direct_lighting_program.get());

						}

					}

				m_direct_lighting_program->unbind();

				glDisable(GL_BLEND);

			}

			// Draw skybox

			glm::mat4 view_without_translation = glm::mat4(glm::mat3(view));
			glDepthFunc(GL_LEQUAL);
			m_skybox->draw(m_projection * view_without_translation);
			glDepthFunc(GL_LESS);

		render_target->unbind();

		if (m_msaa_fb) {
			m_msaa_fb->blit(*m_main_fb, GL_DEPTH_BUFFER_BIT);
			m_msaa_fb->blit(*m_main_fb, GL_COLOR_BUFFER_BIT);
		}

		glDepthMask(GL_TRUE);
		glDisable(GL_DEPTH_TEST);

		// Measure average luminance, then adapt

		_measure_average_luminance();
		_adapt_luminance();

		// Tone mapping

		glm::uvec2 screen_size = m_window.size();
		glViewport(0, 0, screen_size.x, screen_size.y);

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		if (m_tone_map != ToneMap::OFF) {

			m_tone_map_program->bind();
				
				m_tone_map_program->set_texture("u_hdr_texture", m_main_fb->color_texture(0));
				m_tone_map_program->set_uniform("u_exposure", m_exposure);
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

		m_window.swap_buffers();

	}

}