
#ifndef TEXTURE_CUBE_MAP_H
#define TEXTURE_CUBE_MAP_H

#include <array>
#include <memory>

#include "Texture.h"

class TextureCubeMap : public Texture {

	std::array<std::unique_ptr<float[]>, 6> _load_hdr_vertical_cross(const std::string& file_path);

public:

	enum class InputType {
		SPLIT_SIDES, VERTICAL_CROSS//, HORIZONTAL_CROSS, SPHERICAL
	};
	
	TextureCubeMap(const glm::uvec2& size = glm::uvec2(0));

	void upload_ldr(const std::string& folder);
	void upload_hdr(const std::string& path, InputType input_type);

	void upload_hdr_sides(const std::string& folder, const std::string& name);
	void upload_hdr_sides_and_mips(const std::string& folder, const std::string& name, GLuint mip_count);

};

#endif
