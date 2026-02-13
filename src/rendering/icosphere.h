#pragma once

#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "geometry/icosphere.h"
#include "rendering/shader.h"
#include "rendering/camera.h"

namespace IcoSphere {
    const uint32_t MAX_SUBDIVISIONS = 5;
    const uint32_t MAX_INSTANCES = 1000;

    struct BufferEntry {
        uint32_t vertex_index;
        uint32_t index_byte_offset;
        uint32_t vertex_count;
        uint32_t index_count;
    };

    // FUTURE: INSTANCING
    /*
    struct Instance {
        glm::mat4 transform;
    };
    */

    struct Renderer 
    {
        GLuint vao;
        GLuint vertex_buf;
        GLuint index_buf;
        GLuint instance_buf;

        std::vector<float> vertices;
        std::vector<uint32_t> indices;

        BufferEntry lods[MAX_SUBDIVISIONS+1];

        Ouroboros::Shader* shader;
        // FUTURE: INSTANCING
        //Instance instances[MAX_INSTANCES];
    };

    void InitRenderer();
    // FUTURE: INSTANCING
    //void StartBatch();
    void Draw(Camera& cam, int lod, glm::vec3 position, float scale);
    // FUTURE: INSTANCING
    //void EndBatch();
}
