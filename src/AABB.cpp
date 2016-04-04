#include "AABB.h"

AABB::AABB() : minimum(0), maximum(0) {}

AABB::AABB(const glm::vec3 & minimum, const glm::vec3 & maximum) : minimum(minimum), maximum(maximum) {
	assert(glm::all(glm::lessThanEqual(minimum, maximum)));
}

glm::vec3 AABB::center() const {
	return (maximum - minimum) / 2.f + minimum;
}
