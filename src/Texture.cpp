
#include <stb_image.h>

#include "Texture.h"

GLubyte* Texture::load_ldr_image(const std::string& path, int& out_bytes_per_pixel, glm::uvec2& out_size, bool flip_vertically) {

	int width, height;

	FILE* file = fopen(path.c_str(), "rb");

	if (!file) {
		throw std::runtime_error("Texture not found at '" + path +"'.");
	}

	int bytes_per_pixel;

	GLubyte* texture = static_cast<GLubyte*>(stbi_load_from_file(file, &width, &height, &bytes_per_pixel, 0));

	if (bytes_per_pixel != 4) {
		//throw std::runtime_error("PNG textures must be saved with alpha value.");
	}
	
	if (flip_vertically) {

		size_t row_size = width * bytes_per_pixel;

		int half_height = static_cast<int>(std::floor(height / 2.0));
		GLubyte* buffer = new GLubyte[row_size];

		for (int i = 0; i < half_height - 1; i++) {
			memcpy(buffer, texture + i * row_size, row_size);
			memcpy(texture + i * row_size, texture + (height - i - 1) * row_size, row_size);
			memcpy(texture + (height - i - 1) * row_size, buffer, row_size);
		}

		delete[] buffer;

	}

	fclose(file);

	out_bytes_per_pixel = bytes_per_pixel;
	out_size = glm::uvec2(width, height);

	return texture;

}

std::unique_ptr<float[]> Texture::load_hdr_image(const std::string& path, int& component_count, glm::uvec2& out_size) {

	int width, height;

	FILE* file = fopen(path.c_str(), "rb");

	if (!file) {
		throw std::runtime_error("Texture not found at '" + path + "'.");
	}

	float* data = stbi_loadf_from_file(file, &width, &height, &component_count, 0);

	fclose(file);

	out_size = glm::uvec2(width, height);

	size_t size_in_floats = component_count * width * height;

	float* copied_data = new float[size_in_floats];
	std::copy(data, data + size_in_floats, copied_data);

	free(data);

	return std::unique_ptr<float[]>(copied_data);

}

Texture::Texture(GLenum type, const glm::uvec2& size) : m_type(type), m_size(size) {

	assert(type == GL_TEXTURE_2D || type == GL_TEXTURE_CUBE_MAP);

	glGenTextures(1, &m_id);

	if (m_id == 0) {
		throw std::runtime_error("Couldn't create OpenGL texture.");
	}

}

Texture::~Texture() {
	glDeleteTextures(1, &m_id);
}

GLuint Texture::id() const {
	return m_id;
}

const glm::uvec2& Texture::size() const {
	return m_size;
}

void Texture::bind() {
	glBindTexture(m_type, m_id);
}

void Texture::unbind() {
	glBindTexture(m_type, 0);
}

void Texture::set_filters(Filter magnification, Filter minification) const {
	glTexParameteri(m_type, GL_TEXTURE_MAG_FILTER, static_cast<GLint>(magnification));
	glTexParameteri(m_type, GL_TEXTURE_MIN_FILTER, static_cast<GLint>(minification));
}

void Texture::set_wrap_mode(WrapMode s, WrapMode t) const {
	glTexParameteri(m_type, GL_TEXTURE_WRAP_S, static_cast<GLint>(s));
	glTexParameteri(m_type, GL_TEXTURE_WRAP_T, static_cast<GLint>(t));
}

void Texture::set_wrap_mode(WrapMode r, WrapMode s, WrapMode t) const {
	glTexParameteri(m_type, GL_TEXTURE_WRAP_R, static_cast<GLint>(r));
	set_wrap_mode(s, t);
}

void Texture::generate_mipmap() const {
	glGenerateMipmap(m_type);
}
