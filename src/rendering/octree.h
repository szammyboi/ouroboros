#pragma once

#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "shader.h"
#include "rendering/camera.h"

namespace Octree {
	struct Renderer
	{
		GLuint vao;
		GLuint vertex_buf;
		GLuint index_buf;
		GLuint instance_buf;

		Ouroboros::Shader* shader;
	};

	void InitRenderer();
	void Draw(Camera& cam, glm::vec3 position, glm::vec3 scale, float count);
};