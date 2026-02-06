#include "geometry/icosphere.h"

int getMidPoint(std::unordered_map<uint64_t, int>& map, std::vector<float>& vertices, int p1, int p2)
{
	uint64_t smIndex = (p1 < p2) ? p1 : p2;
	uint64_t lgIndex = (p1 < p2) ? p2 : p1;

	uint64_t key = (smIndex << 32) + lgIndex;

	auto [it, inserted] = map.insert({key, vertices.size() / 3});
	if (inserted) {
		glm::vec3& edge0 = glm::vec3(vertices[(p1*3)], vertices[(p1*3)+1], vertices[(p1*3)+2]);
		glm::vec3& edge1 = glm::vec3(vertices[(p2*3)], vertices[(p2*3)+1], vertices[(p2*3)+2]);
		auto midpoint = glm::normalize(edge0 + edge1);
		vertices.push_back(midpoint.x);
		vertices.push_back(midpoint.y);
		vertices.push_back(midpoint.z);
	}

	return it->second;
}

std::vector<uint32_t> subdivide(std::vector<float>& vertices, std::vector<uint32_t>& triangles)
{
	std::unordered_map<uint64_t, int> map;
	std::vector<uint32_t> result;

	uint32_t mid[3];
	for (int i = 0; i < triangles.size(); i+=3)
	{
		for (int edge = 0; edge < 3; edge++)
		{
			mid[edge] = getMidPoint(map, vertices, triangles[i+edge], triangles[i+((edge+1)%3)]);
		}

		result.insert(result.end(), {triangles[i+0], mid[0], mid[2]});
		result.insert(result.end(), {triangles[i+1], mid[1], mid[0]});
		result.insert(result.end(), {triangles[i+2], mid[2], mid[1]});
		result.insert(result.end(), {mid[0], mid[1], mid[2]});
	}

	return result;
}

std::pair<std::vector<float>, std::vector<uint32_t>> GenerateIcoSphere(int subdivision_count)
{
	std::vector<float> vertices = IcoSphere::vertices;
	std::vector<uint32_t> triangles = IcoSphere::triangles;

	for (int i = 0; i < subdivision_count; i++)
		triangles = subdivide(vertices, triangles);

	return {vertices, triangles};
}

