
#ifndef SKYBOX_H
#define SKYBOX_H

#include <array>

#include "FrameBuffer.h"
#include "Program.h"
#include "TextureCubeMap.h"
#include "VertexArray.h"
#include "VertexBuffer.h"

class Skybox {

	static const std::string ROOT_FOLDER;
	static const std::array<glm::vec2, 4> VERTICES_2D;
	static const std::array<glm::vec3, 36> VERTICES_3D;

	std::string m_name;

	Program m_draw_program;
	Program m_precompute_brdf;
	Program m_precompute_diffuse_irradiance;
	Program m_precompute_specular_irradiance;

	TextureCubeMap m_environment_map;

	TextureCubeMap m_diffuse_irradiance_map;
	TextureCubeMap m_specular_irradiance_map;
	Texture2D m_brdf_lut;

	VertexArray m_vao;
	VertexBuffer m_vbo;

	void _precompute_irradiance(bool specular);
	void _precompute_brdf();

public:

	Skybox(const std::string& name);

	void draw(const glm::mat4& view_projection);

	TextureCubeMap& environment_map();
	TextureCubeMap& diffuse_irradiance_map();
	TextureCubeMap& specular_irradiance_map();
	Texture2D& brdf_lut();

};

#endif
