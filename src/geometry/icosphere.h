#pragma once
#include <unordered_map>
#include <vector>
#include <array>
#include <glm/glm.hpp>
#include <iostream>

namespace IcoSphere {
	struct Geometry {
		uint8_t subdivision;
		std::vector<float> vertices;
		std::vector<uint32_t> indices;
	};
}

int getMidPoint(std::unordered_map<uint64_t, int>& map, std::vector<float>& vertices, int p1, int p2);

std::vector<uint32_t> subdivide(std::vector<float>& vertices, std::vector<uint32_t>& triangles);

IcoSphere::Geometry GenerateIcoSphere(uint8_t subdivision_count);
