
#ifndef FULLSCREEN_QUAD_H
#define FULLSCREEN_QUAD_H

#include <glm/glm.hpp>
#include <glad/glad.h>

#include "VertexArray.h"

class FullscreenQuad {

	VertexArray m_vao;
	GLuint m_vbo;

	struct Vertex {
		glm::vec3 p;
		glm::vec2 t;
	};

public:

	FullscreenQuad();
	~FullscreenQuad();

	void draw();
};

#endif
