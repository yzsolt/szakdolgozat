
#include "log.h"
#include "materials.h"

// Texture map

void TextureMap::upload(const std::string& material_directory) {

	if (!path.empty()) {
		texture = std::make_unique<Texture2D>();

		try {
			texture->upload(material_directory + path);
			use_texture = true;
		} catch (const std::runtime_error& e) {
			LOG(std::string("Warning: ") + e.what());
			use_texture = false;
		}

	}

}

// Blinn-Phong material

BlinnPhongMaterial::BlinnPhongMaterial(const std::string& name) : Material(name) {}

void BlinnPhongMaterial::set_uniforms(Program& program) const {

	// Ambient color

	program.set_uniform("u_bpm.ambient.color", ambient.color);

	// Diffuse map

	program.set_uniform("u_bpm.diffuse.color", diffuse.color);
	if (diffuse.texture) {
		program.set_texture("u_bpm.diffuse.texture", *diffuse.texture);
	}
	program.set_uniform("u_bpm.diffuse.use_texture", diffuse.use_texture);

	// Normal map

	if (normal.texture) {
		program.set_texture("u_bpm.normal.texture", *normal.texture);
	}
	program.set_uniform("u_bpm.normal.use_texture", normal.use_texture);

	// Specular map

	program.set_uniform("u_bpm.specular.color", specular.color);
	if (specular.texture) {
		program.set_texture("u_bpm.specular.texture", *specular.texture);
	}
	program.set_uniform("u_bpm.specular.use_texture", specular.use_texture);

	//program.set_uniform("u_bpm.specular.color", specular.color);
	if (reflection.texture) {
		program.set_texture("u_bpm.reflection.texture", *reflection.texture);
	}
	program.set_uniform("u_bpm.reflection.use_texture", reflection.use_texture);

	program.set_uniform("u_bpm.shininess", shininess);

}

void BlinnPhongMaterial::upload(const std::string& material_directory) {

	diffuse.upload(material_directory);
	specular.upload(material_directory);
	normal.upload(material_directory);
	reflection.upload(material_directory);

}

// Physically based material

PhysicallyBasedMaterial::PhysicallyBasedMaterial(const std::string& name) : Material(name) {}

void PhysicallyBasedMaterial::set_uniforms(Program& program) const {

	// Diffuse map

	program.set_uniform("u_pbm.diffuse.color", diffuse.color);
	if (diffuse.texture) {
		program.set_texture("u_pbm.diffuse.texture", *diffuse.texture);
	}
	program.set_uniform("u_pbm.diffuse.use_texture", diffuse.use_texture);

	// Normal map

	if (normal.texture) {
		program.set_texture("u_pbm.normal.texture", *normal.texture);
	}
	program.set_uniform("u_pbm.normal.use_texture", normal.use_texture);

	// Roughness map

	program.set_uniform("u_pbm.roughness.color", roughness.color);
	if (roughness.texture) {
		program.set_texture("u_pbm.roughness.texture", *roughness.texture);
	}
	program.set_uniform("u_pbm.roughness.use_texture", roughness.use_texture);

	// Metalness map

	program.set_uniform("u_pbm.metalness.color", metalness.color);
	if (metalness.texture) {
		program.set_texture("u_pbm.metalness.texture", *metalness.texture);
	}
	program.set_uniform("u_pbm.metalness.use_texture", metalness.use_texture);

}

void PhysicallyBasedMaterial::upload(const std::string& material_directory) {

	diffuse.upload(material_directory);
	normal.upload(material_directory);
	roughness.upload(material_directory);
	metalness.upload(material_directory);

}
