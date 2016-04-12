
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

	TextureMap(const TextureMap& other) : 
		path(other.path), 
		texture(other.texture ? new Texture2D(other.texture->size()) : nullptr), 
		color(other.color), 
		use_texture(other.use_texture) 
	{}

	TextureMap(TextureMap&& other) : path(other.path), texture(std::move(other.texture)), color(other.color), use_texture(other.use_texture) {
	}

	TextureMap(float value) : color(glm::vec4(value, 0, 0, 1)) {}
	TextureMap(const glm::vec3& color) : color(glm::vec4(color, 1)) {}
	TextureMap(const std::string& path, const glm::vec3& color = glm::vec3(0)) : path(path), color(glm::vec4(color, 1)) {}

	TextureMap& operator=(const TextureMap& other) {

		path = other.path;
		texture.reset(other.texture ? new Texture2D(other.texture->size()) : nullptr);
		color = other.color;
		use_texture = other.use_texture;

		return *this;

	}

	void upload(const std::string& material_directory);

};

struct Material {

	/** Name of the material. */
	std::string name;

	/** The diffuse map. */
	TextureMap diffuse;

	/** The normal map. */
	TextureMap normal;

	int use_default = -1;

	Material() {}
	Material(const std::string& name) : name(name) {}
	Material(std::string&& name, TextureMap&& diffuse, TextureMap&& normal) : name(name), diffuse(std::move(diffuse)), normal(std::move(normal)) {}

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

	static const std::vector<PhysicallyBasedMaterial> DEFAULTS;

	/** The roughness map. */
	TextureMap roughness;

	/** The metalness map. */
	TextureMap metalness;

	PhysicallyBasedMaterial(const std::string& name);
	PhysicallyBasedMaterial(
		std::string&& name,
		TextureMap&& diffuse,
		TextureMap&& normal,
		TextureMap&& roughness,
		TextureMap&& metalness
	);

	void set_uniforms(Program& program) const override;
	void upload(const std::string& material_directory) override;

};

#endif
