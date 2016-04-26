
#include "FrameBuffer.h"

std::unordered_map<Texture::InternalFormat, Texture::Format> FrameBuffer::s_internal_format_to_format = {
	{ Texture::InternalFormat::SRGB_8,			Texture::Format::RGB },
	{ Texture::InternalFormat::SRGB_8_A_8,		Texture::Format::RGBA },
	{ Texture::InternalFormat::RGB_8,			Texture::Format::RGB },
	{ Texture::InternalFormat::RGB_8_A_8,		Texture::Format::RGBA },
	{ Texture::InternalFormat::R_16_FLOAT,		Texture::Format::R },
	{ Texture::InternalFormat::RG_16_FLOAT,		Texture::Format::RG },
	{ Texture::InternalFormat::RGB_16_FLOAT,	Texture::Format::RGB },
	{ Texture::InternalFormat::RGBA_16_FLOAT,	Texture::Format::RGBA },
	{ Texture::InternalFormat::RG_32_FLOAT,		Texture::Format::RG },
	{ Texture::InternalFormat::RGB_32_FLOAT,	Texture::Format::RGB },
	{ Texture::InternalFormat::RGBA_32_FLOAT,	Texture::Format::RGBA },
};

std::unordered_map<Texture::InternalFormat, Texture::DataType> FrameBuffer::s_internal_format_to_data_type = {
	{ Texture::InternalFormat::SRGB_8,			Texture::DataType::UNSIGNED_8 },
	{ Texture::InternalFormat::SRGB_8_A_8,		Texture::DataType::UNSIGNED_8 },
	{ Texture::InternalFormat::RGB_8,			Texture::DataType::UNSIGNED_8 },
	{ Texture::InternalFormat::RGB_8_A_8,		Texture::DataType::UNSIGNED_8 },
	{ Texture::InternalFormat::R_16_FLOAT,		Texture::DataType::HALF },
	{ Texture::InternalFormat::RG_16_FLOAT,		Texture::DataType::HALF },
	{ Texture::InternalFormat::RGB_16_FLOAT,	Texture::DataType::HALF },
	{ Texture::InternalFormat::RGBA_16_FLOAT,	Texture::DataType::HALF },
	{ Texture::InternalFormat::RG_32_FLOAT,		Texture::DataType::FLOAT },
	{ Texture::InternalFormat::RGB_32_FLOAT,	Texture::DataType::FLOAT },
	{ Texture::InternalFormat::RGBA_32_FLOAT,	Texture::DataType::FLOAT },
};

FrameBuffer::FrameBuffer(const glm::uvec2& size) :
	m_size(size),
	m_color_attachment_index(0) {
	glGenFramebuffers(1, &m_id);
}

FrameBuffer::~FrameBuffer() {

	glDeleteRenderbuffers(1, &m_depth_render_buffer);
	glDeleteRenderbuffers(m_color_render_buffers.size(), m_color_render_buffers.data());

	glDeleteFramebuffers(1, &m_id);

}

GLuint FrameBuffer::id() const {
	return m_id;
}

const glm::uvec2& FrameBuffer::size() const {
	return m_size;
}

void FrameBuffer::bind() {
	glBindFramebuffer(GL_FRAMEBUFFER, m_id);
}

void FrameBuffer::bind_for_drawing() {

	bind();

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

void FrameBuffer::attach_render_target(AttachmentType attachment_type, Texture::InternalFormat internal_format, GLuint sample_count) {

	GLuint id;

	glGenRenderbuffers(1, &id);

	if (id == 0) {
		throw std::runtime_error("OpenGL render buffer creation failed.");
	}

	bind();
	glBindRenderbuffer(GL_RENDERBUFFER, id);

	if (sample_count > 1) {
		glRenderbufferStorageMultisample(GL_RENDERBUFFER, sample_count, static_cast<GLenum>(internal_format), m_size.x, m_size.y);
	} else {
		glRenderbufferStorage(GL_RENDERBUFFER, static_cast<GLenum>(internal_format), m_size.x, m_size.y);
	}

	GLenum target;

	// TODO: check internal format for depth attachments

	switch (attachment_type) {
	case AttachmentType::DEPTH:
		target = GL_DEPTH_ATTACHMENT;
		break;
	case AttachmentType::DEPTH_STENCIL:
		target = GL_DEPTH_STENCIL_ATTACHMENT;
		break;
	case AttachmentType::COLOR:
		target = GL_COLOR_ATTACHMENT0 + m_color_render_buffers.size();
		break;
	}

	glFramebufferRenderbuffer(GL_FRAMEBUFFER, target, GL_RENDERBUFFER, id);

	if (attachment_type == AttachmentType::COLOR) {
		m_color_render_buffers.push_back(id);
	} else {

		if (m_depth_render_buffer != 0) {
			throw std::runtime_error("A depth render buffer is already attached.");
		}

		m_depth_render_buffer = id;

	}

	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	unbind();

}

void FrameBuffer::blit(const FrameBuffer& to, GLuint bitmask) {

	glBindFramebuffer(GL_READ_FRAMEBUFFER, m_id);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, to.id());

	GLenum filter = ((bitmask & (GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT)) ? GL_NEAREST : GL_LINEAR);
	glBlitFramebuffer(0, 0, m_size.x, m_size.y, 0, 0, to.size().x, to.size().y, bitmask, filter);

}

void FrameBuffer::validate() const {

	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

	if (status != GL_FRAMEBUFFER_COMPLETE) {
		throw std::runtime_error("Something's wrong with an OpenGL framebuffer.");
	}

}
