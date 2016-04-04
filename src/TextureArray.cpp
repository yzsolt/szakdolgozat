
#include "TextureArray.h"

TextureArray::TextureArray(const glm::uvec2& size) : Texture(GL_TEXTURE_2D_ARRAY, size) {

	bind();

	glTexParameteri(m_type, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(m_type, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glTexParameteri(m_type, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(m_type, GL_TEXTURE_WRAP_T, GL_REPEAT);

}

void TextureArray::allocate_storage(InternalFormat internal_format, GLuint mipmap_level_count, GLuint layer_count) {
	glTexStorage3D(m_type, mipmap_level_count, static_cast<GLenum>(internal_format), m_size.x, m_size.y, layer_count);
}

GLuint TextureArray::upload_layer(InternalFormat internal_format, Format format, DataType data_type, const GLvoid * data) {
	glTexSubImage3D(m_type, 0, 0, 0, m_layer_count, m_size.x, m_size.y, 1, static_cast<GLint>(format), static_cast<GLenum>(data_type), data);
	return m_layer_count++;
}
/*
void TextureArray::upload(InternalFormat internal_format, const glm::uvec2 &size, Format format, DataType data_type, const GLvoid *data) {

	glTexImage2D(m_type, 0, static_cast<GLenum>(internal_format), size.x, size.y, 0, static_cast<GLint>(format), static_cast<GLenum>(data_type), data);

}
*/