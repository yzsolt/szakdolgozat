
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
#include "Program.h"
#include "GUI.h"

class Mesh {

	GUI* m_gui;

	GLuint m_vbo;
	GLuint m_ibo;
	std::unique_ptr<VertexArray> m_vao;

	std::string m_directory;

	std::vector<tinyobj::shape_t> m_shapes;
	
	std::vector<bool> m_draw_shape;

	std::vector<GLuint> m_indices;
	std::vector<Vertex> m_vertices;
	
	AABB m_bounding_box;

	bool m_use_pbr = true;
	std::vector<BlinnPhongMaterial> m_bp_materials;
	std::vector<PhysicallyBasedMaterial> m_pb_materials;

	/** Map to match shapes with materials. */
	std::vector<int> m_material_map;

	static glm::vec3 _float3_to_vec3(const float (&values)[3]);
	static Eigen::Vector4f _float3_to_eigen4f(const float (&values)[3]);
	static Eigen::Vector4f _vec4_to_eigen4f(const glm::vec4& value);
	static glm::vec4 _eigen4f_to_vec4(const Eigen::Vector4f& value);

	nanogui::Window* m_materials_window = nullptr;

	void _update_bp_material_info(int material_id);
	void _update_pb_material_info(int material_id);

	void _calculate_tangents();
	//void _load_materials

public:

	/** Load mesh from OBJ file. */
	Mesh(const std::string& path, GUI* gui);

	glm::vec3 center() const;

	void upload();

	void draw(Program& program);

	bool use_pbr() const;

};

#endif
