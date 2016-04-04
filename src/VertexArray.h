
#ifndef VERTEX_ARRAY_H
#define VERTEX_ARRAY_H

#include <memory>

#include <glad/glad.h>

#include "VertexFormatDescriptor.h"

class VertexArray {

	GLuint m_id;
    bool m_bound = false;

    VertexFormatDescriptor m_vertex_format_descriptor;

	//std::unique_ptr<IndexBuffer> m_index_buffer;

public:

    VertexArray();
    ~VertexArray();

    void bind();
    void unbind();

    void set_vertex_format(const VertexFormatDescriptor& vertex_format_descriptor);
	//IndexBuffer& get_index_buffer();

	//void bind_vertex_buffer(const VertexBuffer& vertex_buffer) const;

    void draw_arrays(GLenum mode, GLuint vertex_count) const;
	void draw_arrays(GLenum mode, GLuint from, GLuint vertex_count) const;

	//void draw_indexed() const;
    //void draw_indexed(GLuint from, GLuint count) const;

	//void draw_indexed(IndexBuffer& index_buffer, GLuint from, GLuint count) const;

};

#endif
