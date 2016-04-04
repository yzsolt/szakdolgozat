
#ifndef AABB_H
#define AABB_H

#include <glm/glm.hpp>

struct AABB {

	glm::vec3 minimum;
	glm::vec3 maximum;

	AABB();
	AABB(const glm::vec3& minimum, const glm::vec3& maximum);

	glm::vec3 center() const;

};

#endif
