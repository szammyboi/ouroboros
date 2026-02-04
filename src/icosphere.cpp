#include "icosphere.h"

int getMidPoint(MidPointMap& map, std::vector<glm::vec3>& vertices, int p1, int p2)
{
	uint64_t smIndex = (p1 < p2) ? p1 : p2;
	uint64_t lgIndex = (p1 < p2) ? p2 : p1;

	uint64_t key = (smIndex << 32) + lgIndex;

	auto inserted = map.insert({key, vertices.size()});
	if (inserted.second) {
		glm::vec3& edge0 = vertices[p1];
		glm::vec3& edge1 = vertices[p2];
		auto midpoint = glm::normalize(edge0 + edge1);
		vertices.push_back(midpoint);
	}

	return inserted.second;
}

std::vector<Triangle> subdivide(std::vector<glm::vec3>& vertices, std::vector<Triangle>& triangles)
{
	MidPointMap map;
	std::vector<Triangle> result;
	
	for (auto&& each:triangles)
	{
		std::array<int, 3> mid;
		for (int edge = 0; edge < 3; edge++)
		{
			mid[edge] = getMidPoint(map, vertices, each.indices[edge], each.indices[(edge+1)%3]);
		}

		result.push_back({each.indices[0], mid[0], mid[2]});
		result.push_back({each.indices[1], mid[1], mid[0]});
		result.push_back({each.indices[2], mid[2], mid[1]});
		result.push_back({mid[0], mid[1], mid[2]});
	}

	return result;
}

std::pair<std::vector<glm::vec3>, std::vector<Triangle>> GenerateIcoSphere(int subdivision_count)
{
	std::vector<glm::vec3> vertices = IcoSphere::vertices;
	std::vector<Triangle> triangles = IcoSphere::triangles;

	for (int i = 0; i < subdivision_count; i++)
		triangles = subdivide(vertices, triangles);

	return {vertices, triangles};
}

