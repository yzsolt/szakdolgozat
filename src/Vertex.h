
#ifndef VERTEX_H
#define VERTEX_H

#include <glm/glm.hpp>

#include "VertexFormatDescriptor.h"

struct Vertex {

	glm::vec3 position;
	glm::vec3 normal;
	glm::vec3 tangent;
	glm::vec3 bitangent;
	glm::vec2 texture;

	static VertexFormatDescriptor vertex_format_descriptor() {

		VertexFormatDescriptor vfd;

		vfd.add_attribute(GL_FLOAT, 3, offsetof(Vertex, position));
		vfd.add_attribute(GL_FLOAT, 3, offsetof(Vertex, normal));
		vfd.add_attribute(GL_FLOAT, 3, offsetof(Vertex, tangent));
		vfd.add_attribute(GL_FLOAT, 3, offsetof(Vertex, bitangent));
		vfd.add_attribute(GL_FLOAT, 2, offsetof(Vertex, texture));

		return vfd;

	}

};

#endif
