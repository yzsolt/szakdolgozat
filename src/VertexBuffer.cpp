
#include "VertexBuffer.h"

VertexBuffer::VertexBuffer(VertexBuffer::Usage usage) : m_usage(usage) {

	glGenBuffers(1, &m_id);

	if (m_id == 0) {
		throw std::runtime_error("Couldn't create OpenGL vertex buffer.");
	}

}

VertexBuffer::~VertexBuffer() {
	glDeleteBuffers(1, &m_id);
}

void VertexBuffer::bind() {
	glBindBuffer(GL_ARRAY_BUFFER, m_id);
}

void VertexBuffer::unbind() {
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

size_t VertexBuffer::vertex_count() const {
	return m_vertex_count;
}
