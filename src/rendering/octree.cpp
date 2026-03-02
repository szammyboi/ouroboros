#include "octree.h"
#include "geometry/cube.h"

namespace Octree {
	static Renderer s_Renderer;

	void InitRenderer()
	{
		s_Renderer.shader = new Ouroboros::Shader("src/shaders/octree.vert", "src/shaders/octree.frag");
#ifndef __APPLE__

		glCreateVertexArrays(1, &s_Renderer.vao);
		glCreateBuffers(1, &s_Renderer.vertex_buf);
		glCreateBuffers(1, &s_Renderer.index_buf);
		glCreateBuffers(1, &s_Renderer.instance_buf);

		glNamedBufferStorage(s_Renderer.vertex_buf, Cube::vertices.size() * sizeof(float), Cube::vertices.data(), GL_DYNAMIC_STORAGE_BIT);
		glNamedBufferStorage(s_Renderer.index_buf, Cube::indices.size() * sizeof(uint32_t), Cube::indices.data(), GL_DYNAMIC_STORAGE_BIT);

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
#else
		glGenVertexArrays(1, &s_Renderer.vao);
		glGenBuffers(1, &s_Renderer.vertex_buf);
		glGenBuffers(1, &s_Renderer.index_buf);

		glBindVertexArray(s_Renderer.vao);
		glBindBuffer(GL_ARRAY_BUFFER, s_Renderer.vertex_buf);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, s_Renderer.index_buf);
		glBufferData(GL_ARRAY_BUFFER, Cube::vertices.size() * sizeof(float), Cube::vertices.data(), GL_STATIC_DRAW);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, Cube::indices.size() * sizeof(uint32_t), Cube::indices.data(), GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
#endif
	}

	void Draw(Camera& cam, glm::vec3 position, glm::vec3 scale, float count)
	{
		glm::mat4 model = glm::translate(glm::mat4(1.0), position);
		model = glm::scale(model, scale);

		glBindVertexArray(s_Renderer.vao);
		s_Renderer.shader->SetUniformMatrix4f("model", model);
		s_Renderer.shader->SetUniformMatrix4f("proj", cam.GetProjection());
		s_Renderer.shader->SetUniformMatrix4f("view", cam.GetView());
		s_Renderer.shader->SetUniform1f("count", count);
		glDrawElements(GL_LINES, Cube::indices.size(), GL_UNSIGNED_INT, (void*)0);
		//glDrawElementsInstancedBaseVertexBaseInstance(GL_TRIANGLES, buffer.index_count, GL_UNSIGNED_INT, (void*)buffer.index_byte_offset, 1, buffer.vertex_index, 1);
	}
}
