#pragma once

#include <glm/glm/glm.hpp>

struct Body
{
	glm::vec3 loc { 0.0 };
	float mass { 1.0 };
	float invMass { 1.0 };
	glm::vec3 acc { 0.0 };
	glm::vec3 vel { 0.0 };
	glm::vec3 force { 0.0 };
	glm::vec4 emission { 0.0f };
	float radius = { 1.0 };
	float temperature = 0;
};
