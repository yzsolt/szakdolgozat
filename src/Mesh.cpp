
#include <algorithm>

#include <glm/gtx/string_cast.hpp>

#include "Mesh.h"
#include "log.h"

#undef min
#undef max

using namespace nanogui;

glm::vec3 Mesh::_float3_to_vec3(const float(&values)[3]) {
	return glm::vec3(values[0], values[1], values[2]);
}

Eigen::Vector4f Mesh::_float3_to_eigen4f(const float (&values)[3]) {
	return Eigen::Vector4f(values[0], values[1], values[2], 1.f);
}

Eigen::Vector4f Mesh::_vec4_to_eigen4f(const glm::vec4& value) {
	return Eigen::Vector4f(value[0], value[1], value[2], value[3]);
}

glm::vec4 Mesh::_eigen4f_to_vec4(const Eigen::Vector4f& value) {
	return glm::vec4(value[0], value[1], value[2], value[3]);
}

void Mesh::_update_bp_material_info(int material_id) {

	if (m_bp_materials.empty()) {
		return;
	}

	if (m_materials_window->childCount() > 1) {
		m_materials_window->removeChild(m_materials_window->children()[1]);
	}

	m_materials_window->setTitle("Blinn-Phong materials");

	auto grid_layout = new GridLayout(Orientation::Horizontal, 2, Alignment::Middle, 15, 5);
	grid_layout->setColAlignment({ Alignment::Maximum, Alignment::Fill });
	grid_layout->setSpacing(0, 10);

	auto material_grid = new Widget(m_materials_window);
	material_grid->setLayout(grid_layout);

	TextBox* text_box;
	Widget* split_panel;
	auto& material = m_bp_materials[material_id];

	new Label(material_grid, "Ambient color:", "sans-bold");
	auto ambient_color = new ColorPicker(material_grid, _vec4_to_eigen4f(material.ambient.color));
	ambient_color->setCallback([&material](const Color& color) {
		material.ambient.color = _eigen4f_to_vec4(color);
	});

	new Label(material_grid, "Ambient texture:", "sans-bold");
	split_panel = new Widget(material_grid);
	split_panel->setLayout(new BoxLayout(Orientation::Horizontal, Alignment::Middle, 0, 10));
	text_box = new TextBox(split_panel, material.ambient.path);
	auto use_ambient_texture = new CheckBox(split_panel, "");
	if (material.ambient.path.empty()) {
		text_box->setValue("N/A");
		use_ambient_texture->setChecked(false);
		use_ambient_texture->setEnabled(false);
	} else {
		use_ambient_texture->setChecked(material.ambient.use_texture);
		use_ambient_texture->setCallback([&material](bool value) {
			material.ambient.use_texture = value;
		});
	}

	new Label(material_grid, "Diffuse color:", "sans-bold");
	auto diffuse_color = new ColorPicker(material_grid, _vec4_to_eigen4f(material.diffuse.color));
	diffuse_color->setCallback([&material](const Color& color) {
		material.diffuse.color = _eigen4f_to_vec4(color);
	});

	new Label(material_grid, "Diffuse texture:", "sans-bold");
	split_panel = new Widget(material_grid);
	split_panel->setLayout(new BoxLayout(Orientation::Horizontal, Alignment::Middle, 0, 10));
	text_box = new TextBox(split_panel, material.diffuse.path);
	auto use_diffuse_texture = new CheckBox(split_panel, "");
	if (material.diffuse.path.empty()) {
		text_box->setValue("N/A");
		use_diffuse_texture->setChecked(false);
		use_diffuse_texture->setEnabled(false);
	} else {
		use_diffuse_texture->setChecked(material.diffuse.use_texture);
		use_diffuse_texture->setCallback([&material](bool value) {
			material.diffuse.use_texture = value;
		});
	}

	new Label(material_grid, "Specular color:", "sans-bold");
	auto specular_color = new ColorPicker(material_grid, _vec4_to_eigen4f(material.specular.color));
	specular_color->setCallback([&material](const Color& color) {
		material.specular.color = _eigen4f_to_vec4(color);
	});

	new Label(material_grid, "Specular texture:", "sans-bold");
	split_panel = new Widget(material_grid);
	split_panel->setLayout(new BoxLayout(Orientation::Horizontal, Alignment::Middle, 0, 10));
	text_box = new TextBox(split_panel, material.specular.path);
	auto use_specular_texture = new CheckBox(split_panel, "");
	if (material.specular.path.empty()) {
		text_box->setValue("N/A");
		use_specular_texture->setChecked(false);
		use_specular_texture->setEnabled(false);
	} else {
		use_specular_texture->setChecked(material.specular.use_texture);
		use_specular_texture->setCallback([&material](bool value) {
			material.specular.use_texture = value;
		});
	}

	new Label(material_grid, "Normal texture:", "sans-bold");
	split_panel = new Widget(material_grid);
	split_panel->setLayout(new BoxLayout(Orientation::Horizontal, Alignment::Middle, 0, 10));
	text_box = new TextBox(split_panel, material.normal.path);
	auto use_normal_texture = new CheckBox(split_panel, "");
	if (material.normal.path.empty()) {
		text_box->setValue("N/A");
		use_normal_texture->setChecked(false);
		use_normal_texture->setEnabled(false);
	} else {
		use_normal_texture->setChecked(material.normal.use_texture);
		use_normal_texture->setCallback([&material](bool value) {
			material.normal.use_texture = value;
		});
	}

	new Label(material_grid, "Reflection texture:", "sans-bold");
	split_panel = new Widget(material_grid);
	split_panel->setLayout(new BoxLayout(Orientation::Horizontal, Alignment::Middle, 0, 10));
	text_box = new TextBox(split_panel, material.reflection.path);
	auto use_reflection_texture = new CheckBox(split_panel, "");
	if (material.reflection.path.empty()) {
		text_box->setValue("N/A");
		use_reflection_texture->setChecked(false);
		use_reflection_texture->setEnabled(false);
	} else {
		use_reflection_texture->setChecked(material.reflection.use_texture);
		use_reflection_texture->setCallback([&material](bool value) {
			material.reflection.use_texture = value;
		});
	}

	new Label(material_grid, "Shininess:", "sans-bold");
	auto slider = new Slider(material_grid);
	slider->setValue(material.shininess / 100.f);
	slider->setCallback([&material](float value) {
		material.shininess = value * 100;
	});

	m_gui->performLayout();

}

void Mesh::_update_pb_material_info(int material_id) {

	if (m_pb_materials.empty()) {
		return;
	}

	if (m_materials_window->childCount() > 1) {
		m_materials_window->removeChild(m_materials_window->children()[1]);
	}

	m_materials_window->setTitle("Physically based materials");

	auto grid_layout = new GridLayout(Orientation::Horizontal, 2, Alignment::Middle, 15, 5);
	grid_layout->setColAlignment({ Alignment::Maximum, Alignment::Fill });
	grid_layout->setSpacing(0, 10);

	auto material_grid = new Widget(m_materials_window);
	material_grid->setLayout(grid_layout);

	TextBox* text_box;
	Widget* split_panel;
	auto& material = m_pb_materials[material_id];

	new Label(material_grid, "Diffuse color:", "sans-bold");
	auto diffuse_color = new ColorPicker(material_grid, _vec4_to_eigen4f(material.diffuse.color));
	diffuse_color->setCallback([&material](const Color& color) {
		material.diffuse.color = _eigen4f_to_vec4(color);
	});

	new Label(material_grid, "Diffuse texture:", "sans-bold");
	split_panel = new Widget(material_grid);
	split_panel->setLayout(new BoxLayout(Orientation::Horizontal, Alignment::Middle, 0, 10));
	text_box = new TextBox(split_panel, material.diffuse.path);
	auto use_diffuse_texture = new CheckBox(split_panel, "");
	if (material.diffuse.path.empty()) {
		text_box->setValue("N/A");
		use_diffuse_texture->setChecked(false);
		use_diffuse_texture->setEnabled(false);
	} else {
		use_diffuse_texture->setChecked(material.diffuse.use_texture);
		use_diffuse_texture->setCallback([&material](bool value) {
			material.diffuse.use_texture = value;
		});
	}

	new Label(material_grid, "Normal texture:", "sans-bold");
	split_panel = new Widget(material_grid);
	split_panel->setLayout(new BoxLayout(Orientation::Horizontal, Alignment::Middle, 0, 10));
	text_box = new TextBox(split_panel, material.normal.path);
	auto use_normal_texture = new CheckBox(split_panel, "");
	if (material.normal.path.empty()) {
		text_box->setValue("N/A");
		use_normal_texture->setChecked(false);
		use_normal_texture->setEnabled(false);
	} else {
		use_normal_texture->setChecked(material.normal.use_texture);
		use_normal_texture->setCallback([&material](bool value) {
			material.normal.use_texture = value;
		});
	}

	new Label(material_grid, "Roughness value:", "sans-bold");
	auto roughness_slider = new Slider(material_grid);
	roughness_slider->setValue(material.roughness.color.r);
	roughness_slider->setCallback([&material](float value) {
		material.roughness.color.r = value;
	});

	new Label(material_grid, "Roughness texture:", "sans-bold");
	split_panel = new Widget(material_grid);
	split_panel->setLayout(new BoxLayout(Orientation::Horizontal, Alignment::Middle, 0, 10));
	text_box = new TextBox(split_panel, material.roughness.path);
	auto use_roughness_texture = new CheckBox(split_panel, "");
	if (material.roughness.path.empty()) {
		text_box->setValue("N/A");
		use_roughness_texture->setChecked(false);
		use_roughness_texture->setEnabled(false);
	} else {
		use_roughness_texture->setChecked(material.roughness.use_texture);
		use_roughness_texture->setCallback([&material](bool value) {
			material.roughness.use_texture = value;
		});
	}

	new Label(material_grid, "Metalness value:", "sans-bold");
	auto metalness_slider = new Slider(material_grid);
	metalness_slider->setValue(material.metalness.color.r);
	metalness_slider->setCallback([&material](float value) {
		material.metalness.color.r = value;
	});

	new Label(material_grid, "Metalness texture:", "sans-bold");
	split_panel = new Widget(material_grid);
	split_panel->setLayout(new BoxLayout(Orientation::Horizontal, Alignment::Middle, 0, 10));
	text_box = new TextBox(split_panel, material.metalness.path);
	auto use_metalness_texture = new CheckBox(split_panel, "");
	if (material.metalness.path.empty()) {
		text_box->setValue("N/A");
		use_metalness_texture->setChecked(false);
		use_metalness_texture->setEnabled(false);
	} else {
		use_metalness_texture->setChecked(material.metalness.use_texture);
		use_metalness_texture->setCallback([&material](bool value) {
			material.metalness.use_texture = value;
		});
	}

	m_gui->performLayout();

}

void Mesh::_calculate_tangents() {

	std::vector<Vertex> unindexed_vertices;
	unindexed_vertices.reserve(m_indices.size());

	for (size_t i : m_indices) {
		unindexed_vertices.push_back(m_vertices[i]);
	}

	m_indices.clear();

	for (size_t i = 0; i < unindexed_vertices.size(); i += 3) {

		const glm::vec3& v0 = unindexed_vertices[i].position;

		glm::vec3 v = unindexed_vertices[i + 1].position - v0;
		glm::vec3 w = unindexed_vertices[i + 2].position - v0;

		const glm::vec2& t0 = unindexed_vertices[i].texture;

		glm::vec2 s = unindexed_vertices[i + 1].texture - t0;
		glm::vec2 t = unindexed_vertices[i + 2].texture - t0;
		/*
		float r = 1.0f / (s.x * t.y - s.y * t.x);
		glm::vec3 tangent = (v * t.y - w * s.y) * r;
		glm::vec3 b = (w * s.x - v * t.x) * r;
		*/

		float direction_correction = (s.x * t.y - s.y * t.x) < 0 ? -1 : 1;
		
		if (s.x == 0 && s.y == 0 && t.x == 0 && t.y == 0) {
			s = glm::vec2(0, 1);
			t = glm::vec2(1, 0);
		}

		glm::vec3 tangent = (w * s.y - v * t.y) * direction_correction;
		glm::vec3 bitangent = (w * s.x - v * t.x) * direction_correction;


		//

		for (int j = 0; j < 3; j++) {

			const glm::vec3& n = unindexed_vertices[i + j].normal;

			glm::vec3 local_tangent = glm::normalize(tangent - n * glm::dot(tangent, n));
			glm::vec3 local_bitangent = glm::normalize(bitangent - n * glm::dot(bitangent, n));

			/*if (glm::dot(glm::cross(n, t), b) < 0.0f) {
				t = t * -1.0f;
			}*/

			unindexed_vertices[i + j].tangent = glm::normalize(local_tangent);
			unindexed_vertices[i + j].bitangent = glm::normalize(local_bitangent);
			m_indices.push_back(i + j);

		}

	}

	m_vertices = unindexed_vertices;

}

Mesh::Mesh(const std::string& path, GUI* gui) : m_gui(gui) {

	std::string error_message;
	std::vector<tinyobj::material_t> materials;

	std::size_t last_separator = path.find_last_of("/\\");
	m_directory = path.substr(0, last_separator + 1);
	//std::string file_name = path.substr(last_separator + 1);

	bool success = tinyobj::LoadObj(m_shapes, materials, error_message, path.c_str(), m_directory.c_str());

	if (!success) {
		throw std::runtime_error(error_message);
	}

	GLuint vertex_count = 0;

	for (size_t i = 0; i < m_shapes.size(); i++) {

		for (size_t f = 0; f < m_shapes[i].mesh.indices.size(); f++) {

			const auto& indices = m_shapes[i].mesh.indices;

			m_indices.push_back(indices[f] + vertex_count / 3);

		}

		vertex_count += m_shapes[i].mesh.positions.size();

		for (size_t v = 0; v < m_shapes[i].mesh.positions.size() / 3; v++) {

			const std::vector<float>& p = m_shapes[i].mesh.positions;
			const std::vector<float>& n = m_shapes[i].mesh.normals;
			const std::vector<float>& t = m_shapes[i].mesh.texcoords;

			glm::vec3 position(p[3 * v + 0], p[3 * v + 1], p[3 * v + 2]);
			glm::vec3 normal(n[3 * v + 0], n[3 * v + 1], n[3 * v + 2]);
			glm::vec2 texture(t[2 * v + 0], t[2 * v + 1]);
			
			m_bounding_box.minimum = glm::min(m_bounding_box.minimum, position);
			m_bounding_box.maximum = glm::max(m_bounding_box.maximum, position);

			Vertex vertex;
			vertex.position = position;
			vertex.normal = normal;
			vertex.texture = texture;

			m_vertices.push_back(vertex);

		}

	}

	std::cout << glm::to_string(m_bounding_box.center()) << std::endl;

	m_draw_shape.resize(m_shapes.size(), true);

	_calculate_tangents();

	// Parse materials

	for (const auto& material : materials) {

		// Blinn-Phong materials
		
		m_bp_materials.emplace_back(material.name);
		BlinnPhongMaterial& bpm = m_bp_materials.back();

		bpm.ambient = TextureMap(material.ambient_texname, _float3_to_vec3(material.ambient));
		bpm.diffuse = TextureMap(material.diffuse_texname, _float3_to_vec3(material.diffuse));
		bpm.normal = TextureMap(material.bump_texname, glm::vec3(0));
		bpm.specular = TextureMap(material.specular_texname, _float3_to_vec3(material.specular));
		bpm.reflection = TextureMap(material.reflection_texname, glm::vec3(0));

		bpm.shininess = material.shininess;

		// Physically-based materials

		m_pb_materials.emplace_back(material.name);
		PhysicallyBasedMaterial& pbm = m_pb_materials.back();

		pbm.diffuse = TextureMap(material.diffuse_texname, _float3_to_vec3(material.diffuse));
		pbm.normal = TextureMap(material.bump_texname, glm::vec3(0));
		pbm.roughness = TextureMap(material.specular_texname, _float3_to_vec3(material.specular));
		// TODO: roughness, metalness

	}

	// Build shape <-> material map

	for (size_t i = 0; i < m_shapes.size(); i++) {
		m_material_map.push_back(m_shapes[i].mesh.material_ids.empty() ? -1 : m_shapes[i].mesh.material_ids[0]);
	}

	// Materials window

	m_materials_window = new nanogui::Window(m_gui, "Materials");
	m_materials_window->setLayout(new GroupLayout());

	if (materials.empty()) {
		new Label(m_materials_window, "No materials found.");
	} else {

		std::vector<std::string> material_names;

		for (const auto& material : materials) {
			material_names.push_back(material.name);
		}

		auto* materials = new ComboBox(m_materials_window, material_names);

		materials->setCallback([this](int material_id) {
			if (m_use_pbr) {
				_update_pb_material_info(material_id);
			} else {
				_update_bp_material_info(material_id);
			}
		});

	}

	// Mesh info window

	auto mesh_info_window = new nanogui::Window(m_gui, "Mesh info");
	mesh_info_window->setLayout(new GroupLayout());

	auto use_pbr_checkbox = new CheckBox(mesh_info_window, "Use PBR", [this](bool state) {

		m_use_pbr = state;

		if (m_use_pbr) {
			_update_pb_material_info(0);
		} else {
			_update_bp_material_info(0);
		}

	});
	use_pbr_checkbox->setChecked(m_use_pbr);

	if (m_shapes.empty()) {
		new Label(mesh_info_window, "No shapes found.");
	} else {

		std::vector<std::string> shape_names;

		for (const auto& shape : m_shapes) {
			shape_names.push_back(shape.name);
		}

		auto* shapes = new ComboBox(mesh_info_window, shape_names);

		shapes->setCallback([this](int material_id) {
			//_update_bp_material_info(material_id);
		});

	}
	/*
	FormHelper *shapes_info = new FormHelper(m_gui);
	auto* shapes_window = shapes_info->addWindow(Eigen::Vector2i(120, 5), "Shapes");

	for (size_t i = 0; i < m_shapes.size(); i++) {

		shapes_info->addGroup(m_shapes[i].name);

		bool draw_shape = true;
		size_t vertex_count = m_shapes[i].mesh.positions.size() / 3;
		size_t index_count = m_shapes[i].mesh.indices.size() / 3;

		auto checkbox = shapes_info->addVariable("Draw:", draw_shape);
		checkbox->setCallback([this, i](bool state) {
			m_draw_shape[i] = state;
		});

		shapes_info->addVariable("Vertex count:", vertex_count, false);
		shapes_info->addVariable("Index count:", index_count, false);

	}
	*/
	m_gui->performLayout();

}

glm::vec3 Mesh::center() const {
	return m_bounding_box.center();
}

void Mesh::upload() {

	m_vao = std::make_unique<VertexArray>();

	// Upload vertices and indices

	glGenBuffers(1, &m_vbo);

	if (m_vbo == 0) {
		throw std::runtime_error("Couldn't create the VBO.");
	}

	glGenBuffers(1, &m_ibo);

	if (m_ibo == 0) {
		throw std::runtime_error("Couldn't create the IBO.");
	}
    
	//glDeleteBuffers(1, &m_vbo);

	m_vao->bind();

	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * m_vertices.size(), m_vertices.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * m_indices.size(), m_indices.data(), GL_STATIC_DRAW);

	m_vao->set_vertex_format(Vertex::vertex_format_descriptor());

	// Upload textures

	for (auto& material : m_bp_materials) {

		if (!material.diffuse.path.empty()) {
			material.diffuse.texture = std::make_unique<Texture2D>();
			material.diffuse.texture->upload(m_directory + material.diffuse.path);
			material.diffuse.use_texture = true;
		}

		if (!material.specular.path.empty()) {
			material.specular.texture = std::make_unique<Texture2D>();
			material.specular.texture->upload(m_directory + material.specular.path);
			material.specular.use_texture = true;
		}

		if (!material.normal.path.empty()) {
			material.normal.texture = std::make_unique<Texture2D>();
			material.normal.texture->upload(m_directory + material.normal.path);
			material.normal.use_texture = true;
		}

		if (!material.reflection.path.empty()) {
			material.reflection.texture = std::make_unique<Texture2D>();
			material.reflection.texture->upload(m_directory + material.reflection.path);
			material.reflection.use_texture = true;
		}

	}

	for (auto& material : m_pb_materials) {

		if (!material.diffuse.path.empty()) {
			material.diffuse.texture = std::make_unique<Texture2D>();
			material.diffuse.texture->upload(m_directory + material.diffuse.path);
			material.diffuse.use_texture = true;
		}

		if (!material.normal.path.empty()) {
			material.normal.texture = std::make_unique<Texture2D>();
			material.normal.texture->upload(m_directory + material.normal.path);
			material.normal.use_texture = true;
		}

		if (!material.roughness.path.empty()) {
			material.roughness.texture = std::make_unique<Texture2D>();
			material.roughness.texture->upload(m_directory + material.roughness.path);
			material.roughness.use_texture = true;
		}

	}

	if (m_use_pbr) {
		_update_pb_material_info(0);
	} else {
		_update_bp_material_info(0);
	}

}

void Mesh::draw(Program& program) {

	m_vao->bind();
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);

	//glDrawArrays(GL_TRIANGLES, 0, m_vertices.size());
	//glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT, (void*)0);

	GLuint index_buffer_size = 0;

	for (size_t i = 0; i < m_shapes.size(); i++) {

		if (m_draw_shape[i]) {

			int material_id = m_material_map[i];

			if (material_id > -1) {
				
				if (m_use_pbr) {
					m_pb_materials[material_id].set_uniforms(program);
				} else {
					m_bp_materials[material_id].set_uniforms(program);
				}

			} else {
				// TODO: add a default material
			}

			glDrawElements(GL_TRIANGLES, m_shapes[i].mesh.indices.size(), GL_UNSIGNED_INT, (void*)index_buffer_size);
			//glDrawElementsBaseVertex(GL_TRIANGLES, m_shapes[i].mesh.indices.size(), GL_UNSIGNED_INT, (void*)index_buffer_size, vertex_buffer_size);

		}

		index_buffer_size += sizeof(GLuint) * m_shapes[i].mesh.indices.size();

	}
	
}

bool Mesh::use_pbr() const {
	return m_use_pbr;
}
