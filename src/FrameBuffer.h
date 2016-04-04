
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

	GLuint m_color_attachment_index;

	std::unique_ptr<Texture2D> m_depth_texture;
	std::vector<std::unique_ptr<Texture2D>> m_color_textures;
	std::vector<std::unique_ptr<TextureArray>> m_color_texture_arrays;

	static std::unordered_map<Texture::InternalFormat, Texture::Format> s_internal_format_to_format;
	static std::unordered_map<Texture::InternalFormat, Texture::DataType> s_internal_format_to_data_type;

public:

	FrameBuffer(const glm::uvec2& size);
	~FrameBuffer();

	void bind();
	void unbind();

	Texture2D& color_texture(size_t index);

	void attach_depth_texture();
	size_t attach_color_texture(Texture::InternalFormat internal_format, Texture::Filter filter = Texture::Filter::LINEAR);
	size_t attach_color_texture_array(Texture::InternalFormat internal_format, GLuint layer_count, Texture::Filter filter = Texture::Filter::LINEAR);

	void set_color_texture_level(size_t index, GLuint level, bool change_viewport = true);

	//void activate_draw_on(const std::vector<GLenum>& color_attachments) const;

	void validate() const;

};

#endif
