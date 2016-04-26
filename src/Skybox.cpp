
#include <fstream>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

#include "log.h"
#include "Skybox.h"
#include "Texture2D.h"

const std::string Skybox::ROOT_DIRECTORY = "data/skyboxes/";

const std::array<glm::vec3, 36> Skybox::VERTICES_3D = { {

	{ -1.0f,  1.0f, -1.0f },
	{ -1.0f, -1.0f, -1.0f },
	{ 1.0f, -1.0f, -1.0f },
	{ 1.0f, -1.0f, -1.0f },
	{ 1.0f,  1.0f, -1.0f },
	{ -1.0f,  1.0f, -1.0f },

	{ -1.0f, -1.0f,  1.0f },
	{ -1.0f, -1.0f, -1.0f },
	{ -1.0f,  1.0f, -1.0f },
	{ -1.0f,  1.0f, -1.0f },
	{ -1.0f,  1.0f,  1.0f },
	{ -1.0f, -1.0f,  1.0f },

	{ 1.0f, -1.0f, -1.0f },
	{ 1.0f, -1.0f,  1.0f },
	{ 1.0f,  1.0f,  1.0f },
	{ 1.0f,  1.0f,  1.0f },
	{ 1.0f,  1.0f, -1.0f },
	{ 1.0f, -1.0f, -1.0f },

	{ -1.0f, -1.0f,  1.0f },
	{ -1.0f,  1.0f,  1.0f },
	{ 1.0f,  1.0f,  1.0f },
	{ 1.0f,  1.0f,  1.0f },
	{ 1.0f, -1.0f,  1.0f },
	{ -1.0f, -1.0f,  1.0f },

	{ -1.0f,  1.0f, -1.0f },
	{ 1.0f,  1.0f, -1.0f },
	{ 1.0f,  1.0f,  1.0f },
	{ 1.0f,  1.0f,  1.0f },
	{ -1.0f,  1.0f,  1.0f },
	{ -1.0f,  1.0f, -1.0f },

	{ -1.0f, -1.0f, -1.0f },
	{ -1.0f, -1.0f,  1.0f },
	{ 1.0f, -1.0f, -1.0f },
	{ 1.0f, -1.0f, -1.0f },
	{ -1.0f, -1.0f,  1.0f },
	{ 1.0f, -1.0f,  1.0f },

} };

const std::array<glm::vec2, 4> Skybox::VERTICES_2D = { {
	{ -1.f, -1.f },
	{ -1.f,  1.f },
	{  1.f, -1.f },
	{  1.f,  1.f },
} };

void Skybox::_precompute_irradiance(bool specular) {

	const int CUBEMAP_SPEC_MIPS = 8;

	// Check for existing precomputed textures

	bool generate_textures = false;

	for (GLuint j = 0; j < 6; j++) {

		std::string file_name;

		if (specular) {
			for (GLuint i = 0; i < CUBEMAP_SPEC_MIPS; i++) {

				file_name = m_directory + m_name + ".specular_irradiance." + std::to_string(j) + "." + std::to_string(i) + ".hdr";

				std::ifstream infile(file_name);
				if (!infile.good()) {
					generate_textures = true;
					break;
				}

			}
		} else {

			file_name = m_directory + m_name + ".diffuse_irradiance." + std::to_string(j) + ".hdr";

			std::ifstream infile(file_name);
			if (!infile.good()) {
				generate_textures = true;
				break;
			}

		}

	}

	if (!generate_textures) {
		LOG("Irradiance maps are already generated.");
		return;
	}

	// Generate irradiance maps

	LOG("Generating irradiance maps...");

	VertexFormatDescriptor precompute_vfd;
	precompute_vfd.add_attribute(GL_FLOAT, 2, 0);

	VertexArray temp_vao;
	temp_vao.bind();

	VertexBuffer temp_vbo;
	temp_vbo.bind();

	temp_vao.set_vertex_format(precompute_vfd);
	
	temp_vbo.upload<glm::vec2, 4>(VERTICES_2D);

	glm::uvec2 side_size = specular ? glm::uvec2(512, 512) : glm::uvec2(128, 128);
	
	FrameBuffer irradiance_fb(side_size);

	irradiance_fb.bind();

		irradiance_fb.attach_color_texture(Texture::InternalFormat::RGBA_32_FLOAT);
		irradiance_fb.validate();

		if (specular) {
			irradiance_fb.color_texture(0).generate_mipmap();
		}

		glClearColor(0, 0, 0, 1);
		glClear(GL_COLOR_BUFFER_BIT);

		const glm::vec3 x(1, 0, 0);
		const glm::vec3 y(0, 1, 0);
		const glm::vec3 z(0, 0, 1);

		std::array<glm::mat3, 6> transformations = { {
			glm::mat3(-z, -y, x),
			glm::mat3(z, -y, -x),
			glm::mat3(x, z, y),
			glm::mat3(x, -z, -y),
			glm::mat3(x, -y, z),
			glm::mat3(-x, -y, -z),
		} };

		Program* program = specular ? &m_precompute_specular_irradiance : &m_precompute_diffuse_irradiance;

		program->bind();

			program->set_texture("u_environment_map", m_environment_map);

			for (size_t j = 0; j < 6; j++) {

				program->set_uniform("u_transformation", transformations[j]);

				if (specular) {

					for (GLuint i = 0; i < CUBEMAP_SPEC_MIPS; i++) {

						glm::uvec2 mipmap_size = side_size / unsigned(1 << i);

						irradiance_fb.set_color_texture_level(0, i);
						float roughness = (i + 0.5f) / CUBEMAP_SPEC_MIPS;
						program->set_uniform("u_roughness", roughness);

						temp_vao.draw_arrays(GL_TRIANGLE_STRIP, temp_vbo.vertex_count());

						std::unique_ptr<float[]> buffer(new float[mipmap_size.x * mipmap_size.y * 4]);
						irradiance_fb.color_texture(0).bind();
						glGetTexImage(GL_TEXTURE_2D, i, GL_RGBA, GL_FLOAT, buffer.get());
						std::string output_name = m_directory + m_name + ".specular_irradiance." + std::to_string(j) + "." + std::to_string(i) + ".hdr";
						stbi_write_hdr(output_name.c_str(), mipmap_size.x, mipmap_size.y, 4, buffer.get());

					}

				} else {

					temp_vao.draw_arrays(GL_TRIANGLE_STRIP, temp_vbo.vertex_count());

					std::unique_ptr<float[]> buffer(new float[side_size.x * side_size.y * 4]);
					irradiance_fb.color_texture(0).bind();
					glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_FLOAT, buffer.get());
					std::string output_name = m_directory + m_name + ".diffuse_irradiance." + std::to_string(j) + ".hdr";
					stbi_write_hdr(output_name.c_str(), side_size.x, side_size.y, 4, buffer.get());

				}

			}

		program->unbind();

	irradiance_fb.unbind();
	
	temp_vbo.unbind();
	temp_vao.unbind();

	LOG("Irradiance map generation is done.");
	
}

void Skybox::_precompute_brdf() {

	VertexFormatDescriptor precompute_vfd;
	precompute_vfd.add_attribute(GL_FLOAT, 2, 0);

	VertexArray temp_vao;
	temp_vao.bind();

	VertexBuffer temp_vbo;
	temp_vbo.bind();
	temp_vbo.upload<glm::vec2, 4>(VERTICES_2D);

	temp_vao.set_vertex_format(precompute_vfd);

	glm::uvec2 brdf_size(256, 256);

	FrameBuffer brdf_fb(brdf_size);

	brdf_fb.bind();

		brdf_fb.attach_color_texture(Texture::InternalFormat::RG_32_FLOAT);
		brdf_fb.validate();

		glClearColor(0, 0, 0, 1);
		glClear(GL_COLOR_BUFFER_BIT);

		m_precompute_brdf.bind();
		temp_vao.draw_arrays(GL_TRIANGLE_STRIP, temp_vbo.vertex_count());

		std::unique_ptr<float[]> buffer(new float[brdf_size.x * brdf_size.y * 2]);
		brdf_fb.color_texture(0).bind();

		glGetTexImage(GL_TEXTURE_2D, 0, GL_RG, GL_FLOAT, buffer.get());

		std::string output_name = m_directory + m_name + ".brdf.hdr";
		stbi_write_hdr(output_name.c_str(), brdf_size.x, brdf_size.y, 2, buffer.get());

	brdf_fb.unbind();

	// Upload texture straight from memory

	m_brdf_lut.bind();
		m_brdf_lut.set_wrap_mode(Texture::WrapMode::CLAMP_TO_EDGE, Texture::WrapMode::CLAMP_TO_EDGE);
		m_brdf_lut.upload(Texture::InternalFormat::RG_32_FLOAT, brdf_size, Texture::Format::RG, Texture::DataType::FLOAT, buffer.get());
	m_brdf_lut.unbind();

}

Skybox::Skybox(const std::string& hdr_panorama) : 
	m_draw_program("skybox.vs.glsl", "skybox.fs.glsl") ,
	m_precompute_brdf("precompute_brdf.vs.glsl", "precompute_brdf.fs.glsl"),
	m_precompute_diffuse_irradiance("precompute_irradiance.vs.glsl", "precompute_diffuse_irradiance.fs.glsl"),
	m_precompute_specular_irradiance("precompute_irradiance.vs.glsl", "precompute_specular_irradiance.fs.glsl"),
	m_brdf_lut({ 256, 256 })
{

	reset_panorama(hdr_panorama);

	// Upload vertices

	m_vao.bind();
		m_vbo.bind();

			m_vbo.upload<glm::vec3, 36>(VERTICES_3D);

			VertexFormatDescriptor vfd;
			vfd.add_attribute(GL_FLOAT, 3, 0);

			m_vao.set_vertex_format(vfd);

		m_vbo.unbind();
	m_vao.unbind();

}

void Skybox::reset_panorama(const std::string& hdr_panorama) {

	size_t last_slash_position = hdr_panorama.find_last_of('/');

	if (last_slash_position == std::string::npos) {
		last_slash_position = 0;
	}

	size_t last_backslash_position = hdr_panorama.find_last_of('\\');

	if (last_backslash_position == std::string::npos) {
		last_backslash_position = 0;
	}

	size_t directory_end = std::max(last_slash_position, last_backslash_position);

	m_directory = hdr_panorama.substr(0, directory_end + 1);
	std::string file_name = hdr_panorama.substr(directory_end + 1);
	m_name = file_name.substr(0, file_name.find_last_of('.'));

	m_environment_map.upload_hdr(hdr_panorama, TextureCubeMap::InputType::VERTICAL_CROSS);

	_precompute_irradiance(false);
	_precompute_irradiance(true);
	_precompute_brdf();

	m_diffuse_irradiance_map.upload_hdr_sides(m_directory, m_name + ".diffuse_irradiance");

	//m_specular_irradiance_map.reset(new TextureCubeMap());
	m_specular_irradiance_map.upload_hdr_sides_and_mips(m_directory, m_name + ".specular_irradiance", 8);

}

void Skybox::draw(const glm::mat4& view_projection) {

	m_vao.bind();
		m_vbo.bind();

		m_draw_program.bind();

			m_draw_program.set_texture("u_environment_map", m_environment_map);
			m_draw_program.set_uniform("u_view_projection", view_projection);

			m_vao.draw_arrays(GL_TRIANGLES, m_vbo.vertex_count());

		m_draw_program.unbind();

		m_vbo.unbind();
	m_vao.unbind();

}

TextureCubeMap& Skybox::environment_map() {
	return m_environment_map;
}

TextureCubeMap& Skybox::diffuse_irradiance_map() {
	return m_diffuse_irradiance_map;
}

TextureCubeMap& Skybox::specular_irradiance_map() {
	return m_specular_irradiance_map;
}

Texture2D& Skybox::brdf_lut() {
	return m_brdf_lut;
}
