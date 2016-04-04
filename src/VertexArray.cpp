
#include "VertexArray.h"

VertexArray::VertexArray() {

    glGenVertexArrays(1, &m_id);

	if (m_id == 0) {
		throw std::runtime_error("Failed to create OpenGL vertex array.");
	}

}

VertexArray::~VertexArray() {
    glDeleteVertexArrays(1, &m_id);
}

void VertexArray::bind() {
    glBindVertexArray(m_id);
    m_bound = true;
}

void VertexArray::unbind() {
    glBindVertexArray(0);
    m_bound = false;
}

void VertexArray::set_vertex_format(const VertexFormatDescriptor& vertex_format_descriptor) {
			
	GLuint location = 0;
	GLuint binding_index = 0;

	for (const auto& attribute : vertex_format_descriptor.m_attributes) {

		glEnableVertexAttribArray(location);

		//glVertexAttribFormat(location, attribute.component_count, attribute.type, attribute.normalized, attribute.offset);
		//glVertexAttribBinding(location, binding_index);

		glVertexAttribPointer(location, attribute.component_count, attribute.type, attribute.normalized, vertex_format_descriptor.size(), (void*)attribute.offset);

		location++;

	}

	m_vertex_format_descriptor = vertex_format_descriptor;

}
/*
IndexBuffer& VertexArray::get_index_buffer() {

	if (!m_index_buffer) {

		m_index_buffer = std::make_unique<IndexBuffer>();

		bind();
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_index_buffer->get_id());
		unbind();

	}

	return *m_index_buffer;

}
*/

/*
void VertexArray::bind_vertex_buffer(const VertexBuffer& vertex_buffer) const {
	// TODO: require binding index and check if valid
	glBindVertexBuffer(0, vertex_buffer.get_id(), 0, m_vertex_format_descriptor.get_size());
}
*/
void VertexArray::draw(GLuint vertex_count) const {
    glDrawArrays(GL_TRIANGLES, 0, vertex_count);
}
/*
void VertexArray::draw_indexed() const {
	draw_indexed(0, m_index_buffer->indices().size());
}

void VertexArray::draw_indexed(GLuint from, GLuint count) const {

	assert(m_bound);
	assert(m_index_buffer);
	assert(!m_index_buffer->indices().empty());
	assert(count > 0);

    glDrawRangeElements(GL_TRIANGLES, m_index_buffer->indices().at(from), m_index_buffer->indices().at(from + count - 2), count, GL_UNSIGNED_INT, (void*)(from * sizeof(GLuint)));

}

void VertexArray::draw_indexed(IndexBuffer& index_buffer, GLuint from, GLuint count) const {

	assert(m_bound);
	assert(!index_buffer.indices().empty());
	//assert(count > 0);

	glDrawRangeElements(GL_TRIANGLES, index_buffer.indices().at(from), index_buffer.indices().at(from + count - 2), count, GL_UNSIGNED_INT, (void*)(from * sizeof(GLuint)));

}
*/