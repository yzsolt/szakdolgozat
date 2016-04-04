
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

#include "Skybox.h"
#include "Texture2D.h"

const std::string Skybox::ROOT_FOLDER = "data/skyboxes/";

const std::array<glm::vec3, 36> Skybox::VERTICES = { {

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

void Skybox::_precompute_specular_irradiance() {

	std::array<glm::vec2, 4> temp_vertices = { {
		{ -1.f, -1.f },
		{ -1.f,  1.f },
		{ 1.f, -1.f },
		{ 1.f,  1.f },
	} };

	VertexFormatDescriptor precompute_vfd;
	precompute_vfd.add_attribute(GL_FLOAT, 2, 0);

	VertexArray temp_vao;
	temp_vao.bind();

	GLuint temp_vbo;
	glGenBuffers(1, &temp_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, temp_vbo);

	temp_vao.set_vertex_format(precompute_vfd);

	glm::uvec2 brdf_size(512, 512);
	const int CUBEMAP_SPEC_MIPS = 8;
	
	FrameBuffer specular_irradiance_fb(brdf_size);
	specular_irradiance_fb.bind();

	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * temp_vertices.size(), temp_vertices.data(), GL_STATIC_DRAW);

	specular_irradiance_fb.attach_color_texture(Texture::InternalFormat::RGBA_32_FLOAT);

	specular_irradiance_fb.validate();

	specular_irradiance_fb.color_texture(0).generate_mipmap();

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
		glm::mat3(x, y, -z),
	} };

	m_precompute_specular_irradiance.bind();

		m_precompute_specular_irradiance.set_texture("u_environment_map", m_environment_map);

		for (size_t j = 0; j < 6; j++) {

			m_precompute_specular_irradiance.set_uniform("u_transformation", transformations[j]);

			for (size_t i = 0; i < CUBEMAP_SPEC_MIPS; i++) {

				glm::uvec2 mipmap_size = brdf_size / unsigned(1 << i);

				specular_irradiance_fb.set_color_texture_level(0, i);
				float roughness = (i + 0.5f) / CUBEMAP_SPEC_MIPS;
				m_precompute_specular_irradiance.set_uniform("u_roughness", roughness);
				glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

				std::unique_ptr<float[]> buffer(new float[mipmap_size.x * mipmap_size.y * 4]);
				specular_irradiance_fb.color_texture(0).bind();
				glGetTexImage(GL_TEXTURE_2D, i, GL_RGBA, GL_FLOAT, buffer.get());
				std::string output_name = ROOT_FOLDER + m_name + "/" + m_name + ".specular_irradiance." + std::to_string(j) + "." + std::to_string(i) + ".hdr";
				stbi_write_hdr(output_name.c_str(), mipmap_size.x, mipmap_size.y, 4, buffer.get());
			}

		}

	m_precompute_specular_irradiance.unbind();

	specular_irradiance_fb.unbind();
	
}

void Skybox::_precompute_brdf() {

	std::array<glm::vec2, 4> temp_vertices = { {
		{ -1.f, -1.f },
		{ -1.f,  1.f },
		{ 1.f, -1.f },
		{ 1.f,  1.f },
	} };

	VertexFormatDescriptor precompute_vfd;
	precompute_vfd.add_attribute(GL_FLOAT, 2, 0);

	VertexArray temp_vao;
	temp_vao.bind();

	GLuint temp_vbo;
	glGenBuffers(1, &temp_vbo);

	glBindBuffer(GL_ARRAY_BUFFER, temp_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * temp_vertices.size(), temp_vertices.data(), GL_STATIC_DRAW);

	temp_vao.set_vertex_format(precompute_vfd);

	glm::uvec2 brdf_size(256, 256);

	FrameBuffer brdf_fb(brdf_size);
	brdf_fb.bind();
	brdf_fb.attach_color_texture(Texture::InternalFormat::RGB_32_FLOAT);
	brdf_fb.validate();

	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT);

	m_precompute_brdf.bind();
	glDrawArrays(GL_TRIANGLE_STRIP, 0, temp_vertices.size());

	std::unique_ptr<float[]> buffer(new float[brdf_size.x * brdf_size.y * 3]);
	//brdf_fb.color_texture(0).bind();

	glReadPixels(0, 0, 256, 256, GL_RGB, GL_FLOAT, buffer.get());
	//glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_FLOAT, buffer.get());

	std::string output_name = ROOT_FOLDER + m_name + "/" + m_name + ".brdf.hdr";
	stbi_write_hdr(output_name.c_str(), brdf_size.x, brdf_size.y, 3, buffer.get());

	brdf_fb.unbind();

	m_brdf_lut.bind();
		m_brdf_lut.set_wrap_mode(Texture::WrapMode::CLAMP_TO_EDGE, Texture::WrapMode::CLAMP_TO_EDGE);
		m_brdf_lut.upload(Texture::InternalFormat::RGB_32_FLOAT, brdf_size, Texture::Format::RGB, Texture::DataType::FLOAT, buffer.get());
	m_brdf_lut.unbind();

}

Skybox::Skybox(const std::string& name) : 
	m_name(name),
	m_draw_program("skybox.vs.glsl", "skybox.fs.glsl") ,
	m_precompute_brdf("precompute_brdf.vs.glsl", "precompute_brdf.fs.glsl"),
	m_precompute_specular_irradiance("precompute_specular_irradiance.vs.glsl", "precompute_specular_irradiance.fs.glsl"),
	m_brdf_lut({ 256, 256 })
{

	// Precompute BRDF, diffuse and specular irradiance maps

	

	// Upload texture

	m_environment_map.upload_hdr(ROOT_FOLDER + name + "/" + name + ".hdr", TextureCubeMap::InputType::VERTICAL_CROSS);
	m_diffuse_irradiance_map.upload_hdr(ROOT_FOLDER + name + "/" + name + "_diffuse_irradiance.hdr", TextureCubeMap::InputType::VERTICAL_CROSS);

	_precompute_specular_irradiance();
	_precompute_brdf();

	m_specular_irradiance_map.upload_hdr_sides_and_mips(ROOT_FOLDER + name + "/", name + ".specular_irradiance", 8);

	// Create VBO

	glGenBuffers(1, &m_vbo);

	if (m_vbo == 0) {
		throw std::runtime_error("Couldn't create VBO for the skybox.");
	}

	// Upload vertices

	m_vao.bind();

	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * VERTICES.size(), VERTICES.data(), GL_STATIC_DRAW);

	VertexFormatDescriptor vfd;
	vfd.add_attribute(GL_FLOAT, 3, 0);

	m_vao.set_vertex_format(vfd);

}

void Skybox::draw(const glm::mat4& view_projection) {

	m_vao.bind();
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

	m_draw_program.bind();
	m_draw_program.set_texture("u_environment_map", m_environment_map);
	m_draw_program.set_uniform("u_view_projection", view_projection);

	glDrawArrays(GL_TRIANGLES, 0, VERTICES.size());

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
