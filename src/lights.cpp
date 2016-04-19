
#include "lights.h"

void Light::_set_uniforms(Program& program, const std::string& prefix) const {

	program.set_uniform("u_light_type", static_cast<int>(m_type));

	program.set_uniform(prefix + ".parent.position", m_position);
	program.set_uniform(prefix + ".parent.color", m_color);
	program.set_uniform(prefix + ".parent.luminance", m_luminance);
	program.set_uniform(prefix + ".parent.luminuous_intensity", m_luminous_intensity);
	program.set_uniform(prefix + ".parent.inverse_radius", m_inverse_radius);

}

Light::Light(Type type, const glm::vec3& position, const glm::vec3& color, float luminous_intensity, float luminance) : 
	m_type(type),
	m_position(position),
	m_color(color),
	m_luminous_intensity(luminous_intensity),
	m_luminance(luminance)
{
}

Light::Type Light::type() const {
	return m_type;
}

const glm::vec3& Light::color() const {
	return m_color;
}

void Light::set_color(const glm::vec3& color) {
	m_color = color;
}

const glm::vec3& Light::position() const {
	return m_position;
}

void Light::set_position(const glm::vec3& position) {
	m_position = position;
}

float Light::luminous_intensity() const {
	return m_luminous_intensity;
}

float Light::luminance() const {
	return m_luminance;
}

float Light::inverse_radius() const {
	return m_inverse_radius;
}

bool Light::is_on() const {
	return m_is_on;
}

void Light::set_on(bool on) {
	m_is_on = on;
}

// Point light

PointLight::PointLight(const glm::vec3& position, const glm::vec3& color, float luminous_flux) :
	Light(
		Light::Type::POINT,
		position, 
		color, 
		luminous_flux / glm::two_pi<float>(), 
		luminous_flux / (glm::two_pi<float>() * glm::pi<float>() * 0.0001f)
	)
{
}

void PointLight::set_uniforms(Program& program) const {
	_set_uniforms(program, "u_point_light");
}

// Spot light

SpotLight::SpotLight(const glm::vec3& position, const glm::vec3& color, float luminous_flux, float inner, float outer) :
	Light(
		Light::Type::SPOT,
		position, 
		color,
		luminous_flux / (glm::two_pi<float>() * (1 - std::cos(outer))),
		luminous_flux / (glm::two_pi<float>() * glm::pi<float>() * (1 - std::cos(outer)) * 0.0001f)
	)
{
}

void SpotLight::set_uniforms(Program& program) const {

	_set_uniforms(program, "u_spot_light");

	program.set_uniform("u_spot_light.direction", m_direction);

}

const glm::vec3& SpotLight::direction() const {
	return m_direction;
}

void SpotLight::set_direction(const glm::vec3& direction) {
	m_direction = direction;
}