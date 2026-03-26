#pragma once

#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "geometry/icosphere.h"
#include "rendering/shader.h"
#include "rendering/camera.h"

// max isntance buffer needs to be dynamic based on hardware
namespace IcoSphere {
    struct BufferEntry {
        uint32_t vertex_index;
        uint32_t index_byte_offset;
        uint32_t vertex_count;
        uint32_t index_count;
    };

    struct InstanceEntry {
        glm::mat4 model;
        glm::vec4 color;
    };

    struct LightEntry {
        glm::mat4 model;
        glm::vec4 position;
        glm::vec4 color;
    };

    const uint32_t MAX_SUBDIVISIONS = 5;
    const uint32_t MAX_INSTANCES = 1000;
    const uint32_t MAX_INSTANCE_BUFFER_SIZE = sizeof(InstanceEntry) * 10000;
    const uint32_t MAX_LIGHT_INSTANCE_BUFFER_SIZE = sizeof(LightEntry) * 10000;

    struct Renderer 
    {
        GLuint vao;
        GLuint vertex_buf;
        GLuint index_buf;
        GLuint instance_buf;

        GLuint light_instance_buf;

        std::byte* light_instance_data;
        std::byte* light_instance_ptr;
        uint32_t light_instance_offset;

        std::byte* instance_data;
        std::byte* instance_ptr;
        uint32_t instance_offset;

        std::vector<float> vertices;
        std::vector<uint32_t> indices;

        BufferEntry lods[MAX_SUBDIVISIONS+1];

       

        Ouroboros::Shader* shader;
        Ouroboros::Shader* light_shader;
    };

    void InitRenderer();

    void StartBatch();
    void Submit(glm::mat4 model, glm::vec4 color);
    void SubmitLight(glm::mat4 model, glm::vec3 position, glm::vec4 color);
    void Draw(Camera& cam, int lod, glm::vec3 position, float scale, glm::vec3 lightPos, glm::vec3 color = glm::vec3(1.0f));

    void EndBatch(Camera& cam);
}
