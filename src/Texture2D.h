
#ifndef TEXTURE_2D_H
#define TEXTURE_2D_H

#include "Texture.h"

class Texture2D : public Texture {

public:

	Texture2D(const glm::uvec2& size = glm::uvec2(0));

	void upload(const std::string& path, bool is_srgb = true);

	void upload(InternalFormat internal_format, const glm::uvec2& size, Format format, DataType data_type, const GLvoid* data);

};

#endif
