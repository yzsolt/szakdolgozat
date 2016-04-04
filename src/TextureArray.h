
#ifndef TEXTURE_ARRAY_H
#define TEXTURE_ARRAY_H

#include "Texture.h"

class TextureArray : public Texture {

	GLuint m_layer_count = 0;

public:

	TextureArray(const glm::uvec2& size = glm::uvec2(0));

	void allocate_storage(InternalFormat internal_format, GLuint mipmap_level_count, GLuint layer_count);
	GLuint upload_layer(InternalFormat internal_format, Format format, DataType data_type, const GLvoid* data);

	//void upload(InternalFormat internal_format, const glm::uvec2& size, Format format, DataType data_type, const GLvoid* data);

};

#endif
