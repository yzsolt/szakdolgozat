
#include <array>

#include "FullscreenQuad.h"

FullscreenQuad::FullscreenQuad() {

	glGenBuffers(1, &m_vbo);

	if (m_vbo == 0) {
		throw std::runtime_error("VBO creation has failed.");
	}

	VertexFormatDescriptor vfd;
	vfd.add_attribute(GL_FLOAT, 3, 0);
	vfd.add_attribute(GL_FLOAT, 2, sizeof(glm::vec3));

	std::array<Vertex, 6> vertices = {{
		{ glm::vec3(-1, -1, 0), glm::vec2(0, 0) },
		{ glm::vec3( 1, -1, 0),	glm::vec2(1, 0) },
		{ glm::vec3(-1,  1, 0), glm::vec2(0, 1) },

		{ glm::vec3(-1,  1, 0), glm::vec2(0, 1) },
		{ glm::vec3( 1, -1, 0),	glm::vec2(1, 0) },
		{ glm::vec3( 1,  1, 0),	glm::vec2(1, 1) },
	}};

	m_vao.bind();

	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

	m_vao.set_vertex_format(vfd);

}

FullscreenQuad::~FullscreenQuad() {
	glDeleteBuffers(1, &m_vbo);
}

void FullscreenQuad::draw() {
	m_vao.bind();
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glDrawArrays(GL_TRIANGLES, 0, 6);
}