
#ifndef VERTEX_FORMAT_DESCRIPTOR_H
#define VERTEX_FORMAT_DESCRIPTOR_H

#include <vector>

#include <glad/glad.h>

class VertexFormatDescriptor {

	friend class VertexArray;

    struct Attribute {
        GLuint type;
        GLuint component_count;
        GLuint offset;
        GLboolean normalized;
    };

    std::vector<Attribute> m_attributes;

    size_t m_size;

public:

	VertexFormatDescriptor();

	size_t size() const;

	void add_attribute(GLuint type, GLuint component_count, size_t offset, GLboolean normalized = GL_FALSE);

};

#endif
