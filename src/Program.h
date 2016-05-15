
#ifndef PROGRAM_H
#define PROGRAM_H

#include <list>
#include <forward_list>
#include <unordered_map>

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"
#include "Texture.h"

class Program {

	GLuint m_id;

    bool m_is_linked	= false;
    bool m_is_bound		= false;

    std::vector<GLint> m_attached_shaders;
    std::unordered_map<std::string, GLuint> m_uniforms;

    GLuint _get_location(const std::string& uniform);

public:

    Program();

	/** Convenience constructor - it loads, compiles the shaders and links the program. */
	Program(const std::string& vertex_shader_path, const std::string& fragment_shader_path);

    ~Program();

    void attach_shader(const Shader& shader);
    void detach_shader(const Shader& shader);
    void link();

    void prefetch_uniform_locations(const std::vector<std::string>& uniforms);

    void set_uniform(const std::string& name, GLint value);
    void set_uniform(const std::string& name, float value);

    void set_uniform(const std::string& name, const glm::vec2& vector);
    void set_uniform(const std::string& name, const glm::vec3& vector);
	void set_uniform(const std::string& name, const glm::vec4& vector);

	void set_uniform(const std::string& name, const glm::mat3& matrix, GLuint count = 1);
    void set_uniform(const std::string& name, const glm::mat4& matrix, GLuint count = 1);

	void set_texture(const std::string& texture_uniform, Texture& texture, GLuint unit);

    void bind();
    void unbind();

};

#endif
