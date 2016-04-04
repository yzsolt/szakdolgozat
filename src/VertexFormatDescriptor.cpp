
#include "VertexFormatDescriptor.h"

VertexFormatDescriptor::VertexFormatDescriptor() :
	m_size(0)
{}

size_t VertexFormatDescriptor::size() const {
	return m_size;
}

void VertexFormatDescriptor::add_attribute(GLuint type, GLuint component_count, size_t offset, GLboolean normalized) {

	m_attributes.push_back({ type, component_count, static_cast<GLuint>(offset), normalized });

	size_t type_size;

	switch (type) {

		case GL_HALF_FLOAT:
			type_size = sizeof(GLhalf);
			break;
		case GL_FLOAT:
			type_size = sizeof(GLfloat);
			break;
		case GL_INT_2_10_10_10_REV:
		case GL_UNSIGNED_INT_2_10_10_10_REV:
			type_size = sizeof(GLuint);
			component_count = 1; // Because this is a packed type
			break;
		case GL_UNSIGNED_BYTE:
			type_size = sizeof(GLubyte);
			break;
		case GL_UNSIGNED_SHORT:
			type_size = sizeof(GLushort);
			break;
		case GL_UNSIGNED_INT:
			type_size = sizeof(GLuint);
			break;
		default:
			throw std::runtime_error("Unknown OpenGL type.");

	}

	m_size += type_size * component_count;

}
