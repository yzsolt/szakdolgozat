
#include <algorithm>
#include <memory>

#include "log.h"
#include "Program.h"

Program::Program() {

    m_id = glCreateProgram();

    if (m_id == 0) {
        throw std::runtime_error("Failed to create OpenGL program object.");
    }

}

Program::Program(const std::string& vertex_shader_path, const std::string& fragment_shader_path) : Program() {

	Shader vertex_shader(Shader::Type::VERTEX, vertex_shader_path);
	vertex_shader.compile();

	Shader fragment_shader(Shader::Type::FRAGMENT, fragment_shader_path);
	fragment_shader.compile();

	attach_shader(vertex_shader);
	attach_shader(fragment_shader);

	link();

	LOG("Program created from '" + vertex_shader_path + "' and '" + fragment_shader_path + "' has successfully compiled and linked.");

}

Program::~Program() {
    glDeleteProgram(m_id);
}

void Program::attach_shader(const Shader& shader) {
    glAttachShader(m_id, shader.id());
    m_attached_shaders.push_back(shader.id());
}

void Program::detach_shader(const Shader& shader) {

    glDetachShader(m_id, shader.id());

    m_attached_shaders.erase(
        std::remove(m_attached_shaders.begin(), m_attached_shaders.end(), shader.id()),
        m_attached_shaders.end()
    );

}

void Program::link() {

    glLinkProgram(m_id);

    GLint result;
    glGetProgramiv(m_id, GL_LINK_STATUS, &result);

    if (result == GL_FALSE) {

        glGetProgramiv(m_id, GL_INFO_LOG_LENGTH, &result);

        std::unique_ptr<GLchar[]> message_buffer(new GLchar[result]);
        glGetProgramInfoLog(m_id, result, &result, &message_buffer[0]);
        std::string error_message(message_buffer.get());

        throw std::runtime_error("OpenGL program linking failed with the following message: " + error_message);

    }

    m_is_linked = true;

}

void Program::prefetch_uniform_locations(const std::vector<std::string>& uniforms) {

    assert(m_is_linked);

    for (const std::string& name : uniforms) {
        _get_location(name);
    }

}

void Program::set_uniform(const std::string &name, GLint value) {
    glProgramUniform1i(m_id, this->_get_location(name), value);
}

void Program::set_uniform(const std::string &name, float value) {
    glProgramUniform1f(m_id, this->_get_location(name), value);
}

void Program::set_uniform(const std::string &name, const glm::vec2& value) {
    glProgramUniform2f(m_id, this->_get_location(name), value.x, value.y);
}

void Program::set_uniform(const std::string &name, const glm::vec3& value) {
    glProgramUniform3f(m_id, this->_get_location(name), value.x, value.y, value.z);
}

void Program::set_uniform(const std::string &name, const glm::vec4& value) {
	glProgramUniform4f(m_id, this->_get_location(name), value.x, value.y, value.z, value.w);
}

void Program::set_uniform(const std::string &name, const glm::mat3& matrix, GLuint count) {
	glProgramUniformMatrix3fv(m_id, this->_get_location(name), count, GL_FALSE, glm::value_ptr(matrix));
}

void Program::set_uniform(const std::string &name, const glm::mat4& matrix, GLuint count) {
    glProgramUniformMatrix4fv(m_id, this->_get_location(name), count, GL_FALSE, glm::value_ptr(matrix));
}

void Program::set_texture(const std::string& texture_uniform, Texture& texture) {// , GLuint unit) {

    glActiveTexture(GL_TEXTURE0 + m_texture_unit_counter);
    texture.bind();
    set_uniform(texture_uniform, static_cast<GLint>(m_texture_unit_counter));

	m_texture_unit_counter++;

}

GLuint Program::_get_location(const std::string &uniform) {

    if (m_uniforms.find(uniform) == m_uniforms.end()) {

        GLint location = glGetUniformLocation(m_id, uniform.c_str());

        if (location < 0) {
            //throw std::runtime_error("Couldn't find location for uniform '" + uniform + "' in OpenGL program.");
        }

        m_uniforms[uniform] = location;

    }

    return m_uniforms[uniform];

}

void Program::bind() {
    glUseProgram(m_id);
	m_texture_unit_counter = 0;
    m_is_bound = true;
}

void Program::unbind() {
    assert(m_is_bound);
    glUseProgram(0);
    m_is_bound = false;
}
