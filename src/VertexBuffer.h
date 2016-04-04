
#ifndef VERTEX_BUFFER_H
#define VERTEX_BUFFER_H

#include <array>
#include <vector>

#include <glad/glad.h>

class VertexBuffer {

	enum class Usage {
		STATIC = GL_STATIC_DRAW,
		DYNAMIC = GL_DYNAMIC_DRAW,
		STREAM = GL_STREAM_DRAW
	};

private:

	GLuint m_id = 0;

	Usage m_usage;

	size_t m_vertex_count = 0;

public:

	VertexBuffer(Usage usage = Usage::STATIC);
	~VertexBuffer();

	void bind();
	void unbind();

	size_t vertex_count() const;

	template<typename VertexType>
	void upload(const std::vector<VertexType>& vertices) {
		glBufferData(GL_ARRAY_BUFFER, sizeof(VertexType) * vertices.size(), vertices.data(), static_cast<GLenum>(m_usage));
		m_vertex_count = vertices.size();
	}


	template<typename VertexType, size_t VertexCount>
	void upload(const std::array<VertexType, VertexCount>& vertices) {
		glBufferData(GL_ARRAY_BUFFER, sizeof(VertexType) * vertices.size(), vertices.data(), static_cast<GLenum>(m_usage));
		m_vertex_count = vertices.size();
	}

};

#endif
