
#include <memory>
#include <fstream>
#include <sstream>
#include <regex>

#include "Shader.h"
#include "log.h"

const std::string Shader::ROOT = "shaders/";

std::string Shader::_read_file(const std::string& path) {

	std::ifstream in(path, std::ios::in | std::ios::binary);
	std::string source;

	if (in) {
		std::stringstream content;
		content << in.rdbuf();
		source = content.str();
	} else {
		throw std::runtime_error("Couldn't open shader file at '" + path + "'.");
	}

	// Search for include directives

	std::regex include_regex("#include[ \t]*\"([a-zA-Z0-9_\\-.]+)\"");
	std::smatch include_match;
	std::string source_copy = source;
	size_t include_start = 0;

	while (std::regex_search(source_copy, include_match, include_regex)) {

		if (include_match.size() == 2) {
			std::string include_directive = include_match[0].str();
			include_start = source.find(include_directive, include_start);
			source.replace(include_start, include_directive.length(), _read_file(ROOT + include_match[1].str()));
		}

		source_copy = include_match.suffix().str();

	}

	return source;

}

Shader::Shader(Type type, const std::string& path) : m_type(type), m_path(path) {

	m_id = glCreateShader(static_cast<GLenum>(type));

	if (m_id == 0) {
		throw std::runtime_error("Failed to create OpenGL shader object.");
	}

	std::string source = _read_file(ROOT + path);

	const GLchar* raw_source = source.c_str();

	glShaderSource(m_id, 1, &raw_source, nullptr);

}

Shader::~Shader() {
	glDeleteShader(m_id);
}

GLuint Shader::id() const {
	return m_id;
}

void Shader::compile() {

	glCompileShader(m_id);

	GLint result;
	glGetShaderiv(m_id, GL_COMPILE_STATUS, &result);

	if (result == GL_FALSE) {

		glGetShaderiv(m_id, GL_INFO_LOG_LENGTH, &result);

		std::unique_ptr<GLchar[]> message_buffer(new GLchar[result]);
		glGetShaderInfoLog(m_id, result, &result, &message_buffer[0]);
		std::string error_message(message_buffer.get());

		throw std::runtime_error("Shader compilation failed for shader at '" + m_path + "' with the following message:\n" + error_message);

	}

	m_is_compiled = true;

}

bool Shader::is_compiled() const {
	return m_is_compiled;
}

Shader::Type Shader::type() const {
	return m_type;
}
