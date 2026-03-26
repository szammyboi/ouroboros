#include "octree.h"
#include "geometry/cube.h"
#include "geometry/octree.h"


#include <cstddef>
#include <memory>
#include <new>
#include <type_traits>
#include <queue>

namespace Octree {
	static Renderer s_Renderer;

	void InitRenderer()
	{
		s_Renderer.shader = new Ouroboros::Shader("src/shaders/octree.vert", "src/shaders/octree.frag");

		s_Renderer.instance_data = new std::byte[MAX_INSTANCE_BUFFER_SIZE];

		glCreateVertexArrays(1, &s_Renderer.vao);
		glCreateBuffers(1, &s_Renderer.vertex_buf);
		glCreateBuffers(1, &s_Renderer.index_buf);
		glCreateBuffers(1, &s_Renderer.instance_buf);

		glNamedBufferStorage(s_Renderer.vertex_buf, Cube::vertices.size() * sizeof(float), Cube::vertices.data(), GL_DYNAMIC_STORAGE_BIT);
		glNamedBufferStorage(s_Renderer.index_buf, Cube::indices.size() * sizeof(uint32_t), Cube::indices.data(), GL_DYNAMIC_STORAGE_BIT);
		glNamedBufferStorage(s_Renderer.instance_buf, MAX_INSTANCE_BUFFER_SIZE, s_Renderer.instance_data, GL_DYNAMIC_STORAGE_BIT);

		glVertexArrayVertexBuffer(s_Renderer.vao, 0, s_Renderer.vertex_buf, 0, sizeof(float) * 3);
		glVertexArrayElementBuffer(s_Renderer.vao, s_Renderer.index_buf);

		glVertexArrayAttribFormat(
		    s_Renderer.vao,
		    0,
		    3,
		    GL_FLOAT,
		    GL_FALSE,
		    0);

		glVertexArrayAttribBinding(s_Renderer.vao, 0, 0);
		glEnableVertexArrayAttrib(s_Renderer.vao, 0);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, s_Renderer.instance_buf);
	}

	void StartBatch()
	{
		s_Renderer.instance_ptr = s_Renderer.instance_data;
		s_Renderer.instance_offset = 0;
	}

	void Submit(Box* box) {
		std::queue<Box*> boxes;
		boxes.push(box);

		while (!boxes.empty()) {
			Box* current = boxes.front();
			boxes.pop();

			if (current->object != nullptr)
				Submit(current->pos, current->halfExtent * 2.0f, glm::vec4(0.117f, 0.117f, 0.172f, 1.0f));

			for (Box* child : current->children) {
				if (child)
					boxes.push(child);
			}
		}
	}

	void Submit(glm::vec3 position, glm::vec3 scale, glm::vec4 color)
	{
		glm::mat4 model = glm::translate(glm::mat4(1.0), position);
		model = glm::scale(model, scale);
		
		//InstanceEntry entry = {model, color};
		//std::memcpy(s_Renderer.instance_ptr, &entry, sizeof(InstanceEntry));
		std::construct_at(reinterpret_cast<InstanceEntry*>(s_Renderer.instance_ptr), model, color);
		//std::construct_at<
		s_Renderer.instance_ptr = s_Renderer.instance_ptr + sizeof(InstanceEntry);
		s_Renderer.instance_offset++;
	}

	void EndBatch(Camera& cam)
	{
		glBindVertexArray(s_Renderer.vao);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, s_Renderer.instance_buf);

		size_t instance_bytes = reinterpret_cast<uint8_t*>(s_Renderer.instance_ptr)
                      - reinterpret_cast<uint8_t*>(s_Renderer.instance_data);

		glNamedBufferSubData(
			s_Renderer.instance_buf, 
			0,        
			instance_bytes, 
            s_Renderer.instance_data
		);

		s_Renderer.shader->Use();
		s_Renderer.shader->SetUniformMatrix4f("proj", cam.GetProjection());
		s_Renderer.shader->SetUniformMatrix4f("view", cam.GetView());
		//s_Renderer.shader->SetUniform1i("lod", lod);
		glDrawElementsInstancedBaseVertexBaseInstance(GL_LINES, Cube::indices.size(), GL_UNSIGNED_INT, (void*)0, s_Renderer.instance_offset, 0, 0);
	}

	void Draw(Camera& cam, glm::vec3 position, glm::vec3 scale, glm::vec3 color, float count, bool priority)
	{
		glm::mat4 model = glm::translate(glm::mat4(1.0), position);
		model = glm::scale(model, scale);

		if (priority)
			glDisable(GL_DEPTH_TEST);
		glBindVertexArray(s_Renderer.vao);
		s_Renderer.shader->SetUniformMatrix4f("model", model);
		s_Renderer.shader->SetUniformMatrix4f("proj", cam.GetProjection());
		s_Renderer.shader->SetUniformMatrix4f("view", cam.GetView());
		s_Renderer.shader->SetUniform3f("color", color);
		s_Renderer.shader->SetUniform1f("count", count);
		glDrawElements(GL_LINES, Cube::indices.size(), GL_UNSIGNED_INT, (void*)0);
		if (priority)
			glEnable(GL_DEPTH_TEST);
		//glDrawElementsInstancedBaseVertexBaseInstance(GL_TRIANGLES, buffer.index_count, GL_UNSIGNED_INT, (void*)buffer.index_byte_offset, 1, buffer.vertex_index, 1);
	}
}
