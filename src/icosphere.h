#pragma once
#include <unordered_map>
#include <vector>
#include <glm/glm.hpp>
#include <iostream>

struct Triangle {
	int indices[3];
};

namespace IcoSphere {
	const float X=.525731112119133606f;
	const float Z=.850650808352039932f;
	const float N=0.f;
	
	static const std::vector<glm::vec3> vertices=
	{
		{-X,N,Z}, {X,N,Z}, {-X,N,-Z}, {X,N,-Z},
		{N,Z,X}, {N,Z,-X}, {N,-Z,X}, {N,-Z,-X},
		{Z,X,N}, {-Z,X, N}, {Z,-X,N}, {-Z,-X, N}
	};
	
	static const std::vector<Triangle> triangles=
	{
		{0,4,1},{0,9,4},{9,5,4},{4,5,8},{4,8,1},
		{8,10,1},{8,3,10},{5,3,8},{5,2,3},{2,7,3},
		{7,10,3},{7,6,10},{7,11,6},{11,0,6},{0,1,6},
		{6,1,10},{9,0,11},{9,11,2},{9,2,5},{7,2,11}
	};
}

using MidPointMap = std::unordered_map<uint64_t, int>;

int getMidPoint(MidPointMap& map, std::vector<glm::vec3>& vertices, int p1, int p2);

std::vector<Triangle> subdivide(std::vector<glm::vec3>& vertices, std::vector<Triangle>& triangles);

std::pair<std::vector<glm::vec3>, std::vector<Triangle>> GenerateIcoSphere(int subdivision_count);
