
#include "Texture2D.h"

Texture2D::Texture2D(const glm::uvec2& size) : Texture(GL_TEXTURE_2D, size) {

	bind();

	glTexParameteri(m_type, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(m_type, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glTexParameteri(m_type, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(m_type, GL_TEXTURE_WRAP_T, GL_REPEAT);

}

void Texture2D::upload(InternalFormat internal_format, const glm::uvec2 &size, Format format, DataType data_type, const GLvoid *data) {
	glTexImage2D(m_type, 0, static_cast<GLenum>(internal_format), size.x, size.y, 0, static_cast<GLint>(format), static_cast<GLenum>(data_type), data);
}

void Texture2D::upload(const std::string& path, bool is_srgb) {

	GLubyte* data = Texture::load_ldr_image(path, m_component_count, m_size, true);

	GLint internal_format;
	GLenum format;

	switch (m_component_count) {
	case 4:
		internal_format = is_srgb ? GL_SRGB8_ALPHA8 : GL_RGBA8;
		format = GL_RGBA;
		break;
	case 3:
		internal_format = is_srgb ? GL_SRGB8 : GL_RGB8;
		format = GL_RGB;
		break;
	case 1:
		internal_format = GL_R8;
		format = GL_RED;
		break;
	default:
		throw std::runtime_error("Unknown component count.");
	}

	bind();
	glTexImage2D(m_type, 0, internal_format, m_size.x, m_size.y, 0, format, GL_UNSIGNED_BYTE, data);

	free((void*)data);

}
