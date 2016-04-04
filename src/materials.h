
#ifndef MATERIALS_H
#define MATERIALS_H

#include <string>
#include <memory>

#include <glm/glm.hpp>

#include "Program.h"
#include "Texture2D.h"

struct TextureMap {

	std::string path;
	std::unique_ptr<Texture2D> texture;
	glm::vec4 color = glm::vec4(0.f, 0.f, 0.f, 1.f);
	bool use_texture = false;

	TextureMap() {}
	TextureMap(const std::string& path, const glm::vec3& color) : path(path), color(glm::vec4(color, 1.f)) {}

	void upload(const std::string& material_directory);

};

struct Material {

	/** Name of the material. */
	std::string name;

	/** The diffuse map. */
	TextureMap diffuse;

	/** The normal map. */
	TextureMap normal;

	Material() {}
	Material(const std::string& name) : name(name) {}

	/** Set the relevant material uniforms. */
	virtual void set_uniforms(Program& program) const = 0;

	/** Load textures from the disk and upload them to the GPU. */
	virtual void upload(const std::string& material_directory) = 0;

};

struct BlinnPhongMaterial : public Material {

	/** The ambient map. */
	TextureMap ambient;

	/** The specular map. */
	TextureMap specular;

	/** The reflection map. */
	TextureMap reflection;

	/** Shininess determines how smoothly the specular light distributes on the material. */
	float shininess;

	BlinnPhongMaterial(const std::string& name);

	void set_uniforms(Program& program) const override;
	void upload(const std::string& material_directory) override;

};

struct PhysicallyBasedMaterial : public Material {

	/** The roughness map. */
	TextureMap roughness;

	/** The metalness map. */
	TextureMap metalness;

	PhysicallyBasedMaterial(const std::string& name);

	void set_uniforms(Program& program) const override;
	void upload(const std::string& material_directory) override;

};

#endif
