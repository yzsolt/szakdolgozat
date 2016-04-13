
#ifndef FRAME_BUFFER_H
#define FRAME_BUFFER_H

#include <memory>
#include <vector>
#include <unordered_map>

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "Texture2D.h"
#include "TextureArray.h"

class FrameBuffer {

	GLuint m_id;

	glm::uvec2 m_size;

	GLuint m_color_attachment_index = 0;

	std::unique_ptr<Texture2D> m_depth_texture;
	std::vector<std::unique_ptr<Texture2D>> m_color_textures;
	std::vector<std::unique_ptr<TextureArray>> m_color_texture_arrays;

	GLuint m_depth_render_buffer = 0;
	std::vector<GLuint> m_color_render_buffers;

	static std::unordered_map<Texture::InternalFormat, Texture::Format> s_internal_format_to_format;
	static std::unordered_map<Texture::InternalFormat, Texture::DataType> s_internal_format_to_data_type;

public:

	enum class AttachmentType {
		DEPTH, DEPTH_STENCIL, COLOR
	};

	FrameBuffer(const glm::uvec2& size);
	~FrameBuffer();

	GLuint id() const;
	const glm::uvec2& size() const;

	void bind();
	void bind_for_drawing();
	void unbind();

	Texture2D& color_texture(size_t index);

	void attach_depth_texture();

	size_t attach_color_texture(Texture::InternalFormat internal_format, Texture::Filter filter = Texture::Filter::LINEAR);
	size_t attach_color_texture_array(Texture::InternalFormat internal_format, GLuint layer_count, Texture::Filter filter = Texture::Filter::LINEAR);

	void set_color_texture_level(size_t index, GLuint level, bool change_viewport = true);

	void attach_render_target(AttachmentType attachment_type, Texture::InternalFormat internal_format, GLuint sample_count = 0);
	void blit(const FrameBuffer& to, GLuint bitmask);

	/** Validates the framebuffer for completeness. Throws an exception if the framebuffer is invalid. */
	void validate() const;

};

#endif
