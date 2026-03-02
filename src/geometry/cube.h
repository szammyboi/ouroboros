#pragma once

#include <array>

namespace Cube {
	static const std::array<float, 24> vertices = {
		// Front face
		-0.5f, -0.5f, 0.5f, // 0
		0.5f, -0.5f, 0.5f,  // 1
		0.5f, 0.5f, 0.5f,   // 2
		-0.5f, 0.5f, 0.5f,  // 3
		// Back face
		-0.5f, -0.5f, -0.5f, // 4
		0.5f, -0.5f, -0.5f,  // 5
		0.5f, 0.5f, -0.5f,   // 6
		-0.5f, 0.5f, -0.5f,  // 7
	};

	std::array<unsigned int, 24> indices = {
		// Front face edges
		0, 1, // bottom edge
		1, 2, // right edge
		2, 3, // top edge
		3, 0, // left edge
		// Back face edges
		4, 5, // bottom edge
		5, 6, // right edge
		6, 7, // top edge
		7, 4, // left edge
		// Connecting edges
		0, 4, // bottom-left
		1, 5, // bottom-right
		2, 6, // top-right
		3, 7, // top-left
	};
}