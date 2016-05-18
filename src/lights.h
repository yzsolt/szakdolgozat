
#ifndef LIGHTS_H
#define LIGHTS_H

#include <glm/glm.hpp>

#include "Program.h"

class Light {

public:

	enum class Type {
		POINT	= 0,
		SPOT	= 1,
	};

protected:

	Type m_type;
	
	glm::vec3 m_position;
	glm::vec3 m_color = glm::vec3(1);

	float m_luminous_flux = 1;

	float m_luminous_intensity = 1;
	float m_luminance = 1;

	float m_radius = 1;

	bool m_is_on = false;

	void _set_uniforms(Program& program, const std::string& prefix) const;

	Light(Type type, const glm::vec3& position, const glm::vec3& color, float radius);

public:

	Type type() const;

	virtual void set_uniforms(Program& program) const = 0;

	float luminous_flux() const;
	virtual void set_luminous_flux(float luminous_flux) = 0;

	const glm::vec3& color() const;
	void set_color(const glm::vec3& color);

	const glm::vec3& position() const;
	void set_position(const glm::vec3& position);

	float luminous_intensity() const;
	float luminance() const;

	float radius() const;
	float inverse_radius() const;

	bool is_on() const;
	void set_on(bool on);

};

class PointLight : public Light {

public:

	PointLight(const glm::vec3& position, const glm::vec3& color, float radius, float luminous_flux);

	void set_uniforms(Program& program) const override;
	void set_luminous_flux(float luminous_flux) override;

};

class SpotLight : public Light {

	glm::vec3 m_direction;
	float m_inner_cone_angle;
	float m_outer_cone_angle;

public:

	SpotLight(const glm::vec3& position, const glm::vec3& color, float radius, float luminous_flux, float inner_cone_angle, float outer_cone_angle);

	void set_uniforms(Program& program) const override;
	void set_luminous_flux(float luminous_flux) override;

	const glm::vec3& direction() const;
	void set_direction(const glm::vec3& direction);

	float inner_cone_angle() const;
	void set_inner_cone_angle(float inner_cone_angle);

	float outer_cone_angle() const;
	void set_outer_cone_angle(float outer_cone_angle);

};

#endif
