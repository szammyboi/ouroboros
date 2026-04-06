#pragma once

#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "shader.h"
#include "rendering/camera.h"

struct Box;

namespace Octree {
	struct InstanceEntry {
        glm::mat4 model;
        glm::vec4 color;
    };

	const uint32_t MAX_INSTANCE_BUFFER_SIZE = sizeof(InstanceEntry) * 20000;

	struct Renderer
	{
		GLuint vao;
		GLuint vertex_buf;
		GLuint index_buf;
		GLuint instance_buf;

		std::byte* instance_data;
        std::byte* instance_ptr;
        uint32_t instance_offset;

		Shader* shader;
	};

	void InitRenderer();

	void StartBatch();
	void Submit(Box* box);
    void Submit(glm::vec3 position, glm::vec3 scale, glm::vec4 color);
    void EndBatch(Camera& cam);
}