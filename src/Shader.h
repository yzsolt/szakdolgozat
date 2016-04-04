
#ifndef SHADER_H
#define SHADER_H

#include <string>

#include <glad/glad.h>

class Shader {

public:

	enum class Type : GLenum {
		VERTEX = GL_VERTEX_SHADER,
		FRAGMENT = GL_FRAGMENT_SHADER
	};

	static const std::string ROOT;

private:

	GLuint m_id;
	Type m_type;
	std::string m_path;
	bool m_is_compiled = false;

	static std::string _read_file(const std::string& path);

public:

	Shader(Type type, const std::string& path);
	~Shader();

	GLuint id() const;

	void compile();

	bool is_compiled() const;

	Type type() const;

};

#endif
