
#ifndef TEXTURE_H
#define TEXTURE_H

#include <string>
#include <memory>

#include <glad/glad.h>
#include <glm/glm.hpp>

class Texture {

public:

	enum class Filter : GLint {
		NEAREST = GL_NEAREST,
		LINEAR = GL_LINEAR,
		TRILINEAR = GL_LINEAR_MIPMAP_LINEAR
	};

	enum class WrapMode : GLenum {
		REPEAT = GL_REPEAT,
		CLAMP_TO_EDGE = GL_CLAMP_TO_EDGE
	};

	enum class InternalFormat : GLint {

		RGB_10_A_2 = GL_RGB10_A2,

		RG_16_FLOAT = GL_RG16F,
		RGB_16_FLOAT = GL_RGB16F,
		RGBA_16_FLOAT = GL_RGBA16F,

		RG_32_FLOAT = GL_RG32F,
		RGB_32_FLOAT = GL_RGB32F,
		RGBA_32_FLOAT = GL_RGBA32F,

		R_16_FLOAT = GL_R16F,
		R_32_FLOAT = GL_R32F,

		RGB_8 = GL_RGB8,
		RGB_8_A_8 = GL_RGBA8,

		SRGB_8 = GL_SRGB8,
		SRGB_8_A_8 = GL_SRGB8_ALPHA8,

		DEPTH = GL_DEPTH_COMPONENT,
		DEPTH_16 = GL_DEPTH_COMPONENT16,
		DEPTH_24 = GL_DEPTH_COMPONENT24,
		DEPTH_32 = GL_DEPTH_COMPONENT32,
		DEPTH_32_FLOAT = GL_DEPTH_COMPONENT32F

	};

	enum class Format : GLenum {

		R = GL_RED,
		RG = GL_RG,
		RGB = GL_RGB,
		RGBA = GL_RGBA,
		RGBA_INTEGER = GL_RGBA_INTEGER,

		DEPTH = GL_DEPTH_COMPONENT,

	};

	enum class DataType : GLenum {

		UNSIGNED_8 = GL_UNSIGNED_BYTE,
		UNSIGNED_16 = GL_UNSIGNED_SHORT,
		UNSIGNED_32 = GL_UNSIGNED_INT,

		UNSIGNED_10_10_10_2 = GL_UNSIGNED_INT_10_10_10_2,

		INTEGER_8 = GL_BYTE,
		INTEGER_16 = GL_SHORT,
		INTEGER_32 = GL_INT,

		//INTEGER_10_10_10_2 = GL_INT_2_10_10_10_REV,

		HALF = GL_HALF_FLOAT,
		FLOAT = GL_FLOAT

	};

protected:

	GLuint m_id;
	GLenum m_type;
	glm::uvec2 m_size;
	int m_component_count;

	Texture(GLenum type, const glm::uvec2& size);

	virtual ~Texture();

public:

	static GLubyte* load_ldr_image(const std::string& path, int& component_count, glm::uvec2& out_size, bool flip_vertically = false);
	static std::unique_ptr<float[]> load_hdr_image(const std::string& path, int& component_count, glm::uvec2& out_size);

	GLuint id() const;
	const glm::uvec2& size() const;

	void bind();
	void unbind();

	void set_filters(Filter magnification, Filter minification) const;
	void set_wrap_mode(WrapMode s, WrapMode t) const;
	void set_wrap_mode(WrapMode r, WrapMode s, WrapMode t) const;

	void generate_mipmap() const;

	//void upload(const std::string& path, GLenum internal_format, const glm::uvec2 &size, GLint format, GLenum data_type);

};

#endif
