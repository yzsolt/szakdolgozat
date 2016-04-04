
#include "FrameBuffer.h"

std::unordered_map<Texture::InternalFormat, Texture::Format> FrameBuffer::s_internal_format_to_format = {
	{ Texture::InternalFormat::SRGB_8,			Texture::Format::RGB },
	{ Texture::InternalFormat::SRGB_8_A_8,		Texture::Format::RGBA },
	{ Texture::InternalFormat::RGB_8,			Texture::Format::RGB },
	{ Texture::InternalFormat::RGB_8_A_8,		Texture::Format::RGBA },
	{ Texture::InternalFormat::R_16_FLOAT,		Texture::Format::R },
	{ Texture::InternalFormat::RGB_16_FLOAT,	Texture::Format::RGB },
	{ Texture::InternalFormat::RGBA_16_FLOAT,	Texture::Format::RGBA },
	{ Texture::InternalFormat::RGB_32_FLOAT,	Texture::Format::RGB },
	{ Texture::InternalFormat::RGBA_32_FLOAT,	Texture::Format::RGBA },
};

std::unordered_map<Texture::InternalFormat, Texture::DataType> FrameBuffer::s_internal_format_to_data_type = {
	{ Texture::InternalFormat::SRGB_8,			Texture::DataType::UNSIGNED_8 },
	{ Texture::InternalFormat::SRGB_8_A_8,		Texture::DataType::UNSIGNED_8 },
	{ Texture::InternalFormat::RGB_8,			Texture::DataType::UNSIGNED_8 },
	{ Texture::InternalFormat::RGB_8_A_8,		Texture::DataType::UNSIGNED_8 },
	{ Texture::InternalFormat::R_16_FLOAT,		Texture::DataType::HALF },
	{ Texture::InternalFormat::RGB_16_FLOAT,	Texture::DataType::HALF },
	{ Texture::InternalFormat::RGBA_16_FLOAT,	Texture::DataType::HALF },
	{ Texture::InternalFormat::RGB_32_FLOAT,	Texture::DataType::FLOAT },
	{ Texture::InternalFormat::RGBA_32_FLOAT,	Texture::DataType::FLOAT },
};

FrameBuffer::FrameBuffer(const glm::uvec2& size) :
	m_size(size),
	m_color_attachment_index(0) {
	glGenFramebuffers(1, &m_id);
}

FrameBuffer::~FrameBuffer() {
	glDeleteFramebuffers(1, &m_id);
}

void FrameBuffer::bind() {

	glBindFramebuffer(GL_FRAMEBUFFER, m_id);

	std::vector<GLenum> buffers; // TODO: precalculate
	buffers.reserve(m_color_textures.size());

	for (size_t i = 0; i < m_color_textures.size(); i++) {
		buffers.push_back(GL_COLOR_ATTACHMENT0 + i);
	}

	if (!buffers.empty()) {
		glDrawBuffers(buffers.size(), buffers.data());
	}

	glViewport(0, 0, m_size.x, m_size.y);

}

void FrameBuffer::unbind() {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

Texture2D& FrameBuffer::color_texture(size_t index) {
	return *m_color_textures[index];
}

void FrameBuffer::attach_depth_texture() {

	if (m_depth_texture) {
		throw std::runtime_error("A depth texture is already attached to this frame buffer!");
	}

	m_depth_texture = std::make_unique<Texture2D>(m_size);
	m_depth_texture->bind();
	m_depth_texture->upload(
		Texture::InternalFormat::DEPTH_32,
		m_size,
		Texture::Format::DEPTH,
		Texture::DataType::FLOAT,
		nullptr
	);
	m_depth_texture->set_filters(Texture::Filter::LINEAR, Texture::Filter::LINEAR);
	m_depth_texture->set_wrap_mode(Texture::WrapMode::CLAMP_TO_EDGE, Texture::WrapMode::CLAMP_TO_EDGE);

	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_depth_texture->id(), 0);

}

size_t FrameBuffer::attach_color_texture(Texture::InternalFormat internal_format, Texture::Filter filter) {

	// TODO: check color attachment index limit

	auto color_texture = std::make_unique<Texture2D>(m_size);
	color_texture->bind();
	color_texture->upload(
		internal_format,
		m_size,
		s_internal_format_to_format[internal_format],
		s_internal_format_to_data_type[internal_format],
		nullptr
	);
	color_texture->set_filters(Texture::Filter::LINEAR, Texture::Filter::LINEAR);
	color_texture->set_wrap_mode(Texture::WrapMode::CLAMP_TO_EDGE, Texture::WrapMode::CLAMP_TO_EDGE);

	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + m_color_attachment_index++, color_texture->id(), 0);

	m_color_textures.push_back(std::move(color_texture));

	//return GL_COLOR_ATTACHMENT0 + m_color_attachment_index - 1;
	return m_color_textures.size() - 1;

}

size_t FrameBuffer::attach_color_texture_array(Texture::InternalFormat internal_format, GLuint layer_count, Texture::Filter filter) {

	// TODO: check color attachment index limit

	auto color_texture = std::make_unique<TextureArray>(m_size);
	color_texture->bind();
	color_texture->allocate_storage(
		internal_format,
		0,
		layer_count
	);
	color_texture->set_filters(Texture::Filter::LINEAR, Texture::Filter::LINEAR);
	color_texture->set_wrap_mode(Texture::WrapMode::CLAMP_TO_EDGE, Texture::WrapMode::CLAMP_TO_EDGE);

	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + m_color_attachment_index++, color_texture->id(), 0);

	m_color_texture_arrays.push_back(std::move(color_texture));

	//return GL_COLOR_ATTACHMENT0 + m_color_attachment_index - 1;
	return m_color_texture_arrays.size() - 1;

}

void FrameBuffer::set_color_texture_level(size_t index, GLuint level, bool change_viewport) {

	assert(index < m_color_textures.size());

	auto& texture = m_color_textures[index];

	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, texture->id(), level);

	if (change_viewport) {
		glm::uvec2 mip_size = texture->size() / static_cast<GLuint>(1 << level);
		glViewport(0, 0, mip_size.x, mip_size.y);
	}

}

/*
void FrameBuffer::activate_draw_on(const std::vector<GLenum>& color_attachments) const {
	glDrawBuffers(static_cast<GLsizei>(color_attachments.size()), color_attachments.data());
}
*/
void FrameBuffer::validate() const {

	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

	if (status != GL_FRAMEBUFFER_COMPLETE) {
		throw std::runtime_error("Something's wrong with an OpenGL framebuffer.");
	}

}
