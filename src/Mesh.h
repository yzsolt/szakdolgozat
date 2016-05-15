
#ifndef MESH_H
#define MESH_H

#include <string>
#include <vector>

#include <glad/glad.h>
#include "tiny_obj_loader.h"

#include "AABB.h"
#include "materials.h"
#include "Vertex.h"
#include "VertexArray.h"
#include "VertexBuffer.h"
#include "Program.h"
#include "GUI.h"

class Mesh {

	GUI* m_gui;
	
	VertexArray m_vao;
	VertexBuffer m_vbo;
	GLuint m_ibo;

	std::string m_directory;

	std::vector<tinyobj::shape_t> m_shapes;
	
	std::vector<bool> m_draw_shape;

	std::vector<GLuint> m_indices;
	std::vector<Vertex> m_vertices;
	
	AABB m_bounding_box;
	float m_scale = 1.f;

	bool m_use_pbr = true;

	std::vector<BlinnPhongMaterial> m_bp_materials;
	std::vector<PhysicallyBasedMaterial> m_pb_materials;
	std::vector<PhysicallyBasedMaterial> m_default_pb_materials;

	/** Map to match shapes with materials. */
	std::vector<int> m_material_map;

	static glm::vec3 _float3_to_vec3(const float (&values)[3]);
	static Eigen::Vector4f _float3_to_eigen4f(const float (&values)[3]);
	static Eigen::Vector4f _vec4_to_eigen4f(const glm::vec4& value);
	static glm::vec4 _eigen4f_to_vec4(const Eigen::Vector4f& value);

	// Material info window

	nanogui::Window* m_bp_materials_window = nullptr;
	nanogui::Window* m_pb_materials_window = nullptr;

	size_t m_selected_material = 0;
	nanogui::ComboBox* m_predefined_materials_button = nullptr;
	nanogui::CheckBox* m_use_predefined_material_checkbox = nullptr;

	void _update_bp_material_info(int material_id);
	void _update_pb_material_info(int material_id, bool predefined = false);

	// Shape info window

	nanogui::Window* m_mesh_info_window = nullptr;

	void _update_mesh_info(int shape_id);

	void _calculate_tangents();
	//void _load_materials

public:

	/** Load mesh from OBJ file. */
	Mesh(const std::string& path, GUI* gui);
	~Mesh();

	glm::vec3 center() const;

	void upload();

	void draw(Program* program = nullptr);

	float scale() const;

	bool use_pbr() const;

};

#endif
