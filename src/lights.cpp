
#include "lights.h"

void Light::_set_uniforms(Program& program, const std::string& prefix) const {

	program.set_uniform("u_light_type", static_cast<int>(m_type));

	program.set_uniform(prefix + ".parent.position", m_position);
	program.set_uniform(prefix + ".parent.color", m_color);
	program.set_uniform(prefix + ".parent.luminance", m_luminance);
	program.set_uniform(prefix + ".parent.luminous_intensity", m_luminous_intensity);
	program.set_uniform(prefix + ".parent.inverse_radius", inverse_radius());

}

Light::Light(Type type, const glm::vec3& position, const glm::vec3& color, float radius) : 
	m_type(type),
	m_position(position),
	m_color(color),
	m_radius(radius)
{
}

Light::Type Light::type() const {
	return m_type;
}

float Light::luminous_flux() const {
	return m_luminous_flux;
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

float Light::radius() const {
	return m_radius;
}

float Light::inverse_radius() const {
	return m_radius == 0 ? FLT_MAX : 1.f / m_radius;
}

bool Light::is_on() const {
	return m_is_on;
}

void Light::set_on(bool on) {
	m_is_on = on;
}

// Point light

PointLight::PointLight(const glm::vec3& position, const glm::vec3& color, float radius, float luminous_flux) :
	Light(Light::Type::POINT, position, color, radius)
{
	set_luminous_flux(luminous_flux);
}

void PointLight::set_uniforms(Program& program) const {
	_set_uniforms(program, "u_point_light");
}

void PointLight::set_luminous_flux(float luminous_flux) {

	m_luminous_flux = luminous_flux;

	m_luminous_intensity = luminous_flux / glm::two_pi<float>();
	m_luminance = luminous_flux / (glm::two_pi<float>() * glm::pi<float>() * 0.0001f);

}

// Spot light

SpotLight::SpotLight(const glm::vec3& position, const glm::vec3& color, float radius, float luminous_flux, float inner_cone_angle, float outer_cone_angle) :
	Light(Light::Type::SPOT, position, color, radius),
	m_inner_cone_angle(inner_cone_angle),
	m_outer_cone_angle(outer_cone_angle)
{
	assert(outer_cone_angle > inner_cone_angle);
	set_luminous_flux(luminous_flux);
}

void SpotLight::set_uniforms(Program& program) const {

	_set_uniforms(program, "u_spot_light");

	program.set_uniform("u_spot_light.direction", m_direction);
	program.set_uniform("u_spot_light.inner_cone_angle", std::cos(m_inner_cone_angle));
	program.set_uniform("u_spot_light.outer_cone_angle", std::cos(m_outer_cone_angle));

}

void SpotLight::set_luminous_flux(float luminous_flux) {

	m_luminous_flux = luminous_flux;

	m_luminous_intensity = luminous_flux / (glm::two_pi<float>() * (1 - std::cos(m_outer_cone_angle)));
	m_luminance = luminous_flux / (glm::two_pi<float>() * glm::pi<float>() * (1 - std::cos(m_outer_cone_angle)) * 0.0001f);

}

const glm::vec3& SpotLight::direction() const {
	return m_direction;
}

void SpotLight::set_direction(const glm::vec3& direction) {
	m_direction = direction;
}

float SpotLight::inner_cone_angle() const {
	return m_inner_cone_angle;
}

void SpotLight::set_inner_cone_angle(float inner_cone_angle) {
	assert(inner_cone_angle <= m_outer_cone_angle);
	m_inner_cone_angle = inner_cone_angle;
}

float SpotLight::outer_cone_angle() const {
	return m_outer_cone_angle;
}

void SpotLight::set_outer_cone_angle(float outer_cone_angle) {
	assert(outer_cone_angle >= m_inner_cone_angle);
	m_outer_cone_angle = outer_cone_angle;
}
