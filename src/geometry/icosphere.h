#pragma once
#include <unordered_map>
#include <vector>
#include <array>
#include <glm/glm.hpp>
#include <iostream>

const float GOLDEN_RATIO = 1.61803398875f;

namespace IcoSphere {
	const float X = 1.0f / sqrt(1 + GOLDEN_RATIO * GOLDEN_RATIO);
	const float Z = GOLDEN_RATIO * X;
	const float N = 0.0f;
	
	static const std::vector<float> vertices=
	{
		-X,N,Z, X,N,Z, -X,N,-Z, X,N,-Z,
		N,Z,X, N,Z,-X, N,-Z,X, N,-Z,-X,
		Z,X,N, -Z,X, N, Z,-X,N, -Z,-X, N
	};
	
	static const std::vector<uint32_t> triangles =
	{
		0,4,1,  0,9,4,  9,5,4,  4,5,8,  4,8,1,
		8,10,1, 8,3,10, 5,3,8, 5,2,3, 2,7,3,
		7,10,3, 7,6,10, 7,11,6, 11,0,6, 0,1,6,
		6,1,10, 9,0,11, 9,11,2, 9,2,5, 7,2,11
	};

}

int getMidPoint(std::unordered_map<uint64_t, int>& map, std::vector<float>& vertices, int p1, int p2);

std::vector<uint32_t> subdivide(std::vector<float>& vertices, std::vector<uint32_t>& triangles);

std::pair<std::vector<float>, std::vector<uint32_t>> GenerateIcoSphere(int subdivision_count);
