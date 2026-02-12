#include "rendering/icosphere.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace IcoSphere {
    static Renderer s_Renderer;
    static uint32_t s_Count;

    void InitRenderer()
    {
        s_Renderer.shader = new Ouroboros::Shader("src/shaders/ico.vert", "src/shaders/ico.frag");

        int vertex_index_offset = 0;
        int index_byte_offset = 0;
        for (int i = 0; i <= MAX_SUBDIVISIONS; i++)
        {
            Geometry geometry = GenerateIcoSphere(i);

            BufferEntry& data = s_Renderer.lods[i];

            data.vertex_index = vertex_index_offset;
            data.index_byte_offset = index_byte_offset;
            data.vertex_count = geometry.vertices.size() / 3;
            data.index_count = geometry.indices.size();
            
            s_Renderer.vertices.insert(s_Renderer.vertices.end(), geometry.vertices.begin(), geometry.vertices.end());
            s_Renderer.indices.insert(s_Renderer.indices.end(), geometry.indices.begin(), geometry.indices.end());

            vertex_index_offset += data.vertex_count;
            index_byte_offset += data.index_count * sizeof(uint32_t);
        }

        glCreateVertexArrays(1, &s_Renderer.vao);
        glCreateBuffers(1, &s_Renderer.vertex_buf);
        glCreateBuffers(1, &s_Renderer.index_buf);
        glCreateBuffers(1, &s_Renderer.instance_buf);

        glNamedBufferStorage(s_Renderer.vertex_buf, s_Renderer.vertices.size() * sizeof(float), s_Renderer.vertices.data(), GL_DYNAMIC_STORAGE_BIT);
        glNamedBufferStorage(s_Renderer.index_buf, s_Renderer.indices.size() * sizeof(uint32_t), s_Renderer.indices.data(), GL_DYNAMIC_STORAGE_BIT);
        
        // FUTURE: INSTANCING
        // glNamedBufferStorage(s_Renderer.instance_buf, MAX_INSTANCES * sizeof(Instance), s_Renderer.instances, GL_DYNAMIC_STORAGE_BIT);

        glVertexArrayVertexBuffer(s_Renderer.vao, 0, s_Renderer.vertex_buf, 0, sizeof(float) * 3);
        glVertexArrayElementBuffer(s_Renderer.vao, s_Renderer.index_buf);

        glVertexArrayAttribFormat(
            s_Renderer.vao,
            0,
            3,
            GL_FLOAT,
            GL_FALSE,
            0
        );

        glVertexArrayAttribBinding(s_Renderer.vao, 0, 0);
        glEnableVertexArrayAttrib(s_Renderer.vao, 0);

        // FUTURE: INSTANCING
        // glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, instance_buffer);
    }
    // TO IMPLEMENT: SCALE
    void Draw(int lod, glm::vec3 position, float scale)
    {
        glm::vec3 cam_pos = glm::vec3(0.0, 0.0, 3.0f);
        glm::mat4 view = glm::lookAt(cam_pos, 
            glm::vec3(0.0f, 0.0f, 0.0f), 
            glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 proj = glm::perspective(glm::radians(45.0f), 1200.0f / 1200.0f, 1.0f, 400.0f);
            
        BufferEntry buffer = s_Renderer.lods[lod];
        glm::mat4 model = glm::translate(glm::mat4(1.0), position);
        
        glBindVertexArray(s_Renderer.vao);
        s_Renderer.shader->SetUniformMatrix4f("model", model);
        s_Renderer.shader->SetUniformMatrix4f("proj", proj);
        s_Renderer.shader->SetUniformMatrix4f("view", view);
        glDrawElementsBaseVertex(GL_TRIANGLES, buffer.index_count, GL_UNSIGNED_INT, (void*)buffer.index_byte_offset, buffer.vertex_index);
        //glDrawElementsInstancedBaseVertexBaseInstance(GL_TRIANGLES, buffer.index_count, GL_UNSIGNED_INT, (void*)buffer.index_byte_offset, 1, buffer.vertex_index, 1);
        s_Count++;
    }
}