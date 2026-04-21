#include "rendering/icosphere.h"
#include "rendering/camera.h"
#include "global.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <cstddef>
#include <memory>
#include <new>
#include <type_traits>

namespace IcoSphere {
	static Renderer s_Renderer;
	static uint32_t s_Count;

	void InitRenderer(glm::vec2 resolution)
	{
		s_Renderer.shader = new Shader("src/shaders/ico.vert", "src/shaders/ico.frag");
		s_Renderer.light_shader = new Shader("src/shaders/light.vert", "src/shaders/light.frag");
		s_Renderer.pass1 = new Shader("src/shaders/quad.vert", "src/shaders/jwst/pass1.frag");
		s_Renderer.pass2 = new Shader("src/shaders/quad.vert", "src/shaders/jwst/pass2.frag");
		s_Renderer.finalpass = new Shader("src/shaders/quad.vert", "src/shaders/jwst/render.frag");
		s_Renderer.grain = new Shader("src/shaders/quad.vert", "src/shaders/grain.frag");

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

		s_Renderer.instance_data = new std::byte[MAX_INSTANCE_BUFFER_SIZE];
		s_Renderer.light_instance_data = new std::byte[MAX_INSTANCE_BUFFER_SIZE];

		glCreateVertexArrays(1, &s_Renderer.vao);
		glCreateBuffers(1, &s_Renderer.vertex_buf);
		glCreateBuffers(1, &s_Renderer.index_buf);
		glCreateBuffers(1, &s_Renderer.instance_buf);
		glCreateBuffers(1, &s_Renderer.light_instance_buf);

		glNamedBufferStorage(s_Renderer.vertex_buf, s_Renderer.vertices.size() * sizeof(float), s_Renderer.vertices.data(), GL_DYNAMIC_STORAGE_BIT);
		glNamedBufferStorage(s_Renderer.index_buf, s_Renderer.indices.size() * sizeof(uint32_t), s_Renderer.indices.data(), GL_DYNAMIC_STORAGE_BIT);
		glNamedBufferStorage(s_Renderer.instance_buf, MAX_INSTANCE_BUFFER_SIZE, s_Renderer.instance_data, GL_DYNAMIC_STORAGE_BIT);
		glNamedBufferStorage(s_Renderer.light_instance_buf, MAX_LIGHT_INSTANCE_BUFFER_SIZE, s_Renderer.light_instance_data, GL_DYNAMIC_STORAGE_BIT);

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
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, s_Renderer.light_instance_buf);

		s_Renderer.postprocessing = new RenderSequence(
			resolution,
			{
				{
					s_Renderer.pass1,
					glm::vec2(1.0f, 1.0f)
				},
				{
					s_Renderer.pass2,
					glm::vec2(1.0f, 1.0f)
				},
				{
					s_Renderer.finalpass,
					glm::vec2(1.0f, 1.0f)
				},
				{
					s_Renderer.grain,
					glm::vec2(1.0f, 1.0f)
				}
			}
		);
	}

	void Resize(glm::vec2 resolution)
	{
		s_Renderer.postprocessing->Resize(resolution);
	}

	void StartBatch()
	{
		s_Renderer.instance_ptr = s_Renderer.instance_data;
		s_Renderer.instance_offset = 0;

		s_Renderer.light_instance_ptr = s_Renderer.light_instance_data;
		s_Renderer.light_instance_offset = 0;

		s_Renderer.postprocessing->SetupMainImage();
	}

	void Submit(glm::mat4 model, glm::vec4 color)
	{
		std::construct_at(reinterpret_cast<InstanceEntry*>(s_Renderer.instance_ptr), model, color);
		s_Renderer.instance_ptr = s_Renderer.instance_ptr + sizeof(InstanceEntry);
		s_Renderer.instance_offset++;
	}

	void SubmitLight(glm::mat4 model, glm::vec3 position, glm::vec4 color)
	{
		std::construct_at(reinterpret_cast<LightEntry*>(s_Renderer.light_instance_ptr), model, glm::vec4(position, 500.0),color);
		s_Renderer.light_instance_ptr = s_Renderer.light_instance_ptr + sizeof(LightEntry);
		s_Renderer.light_instance_offset++;
	}

	void EndBatch(Camera& cam)
	{
		glBindVertexArray(s_Renderer.vao);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, s_Renderer.instance_buf);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, s_Renderer.light_instance_buf);

		size_t instance_bytes = reinterpret_cast<uint8_t*>(s_Renderer.instance_ptr)
                      - reinterpret_cast<uint8_t*>(s_Renderer.instance_data);

		size_t light_instance_bytes = reinterpret_cast<uint8_t*>(s_Renderer.light_instance_ptr)
                      - reinterpret_cast<uint8_t*>(s_Renderer.light_instance_data);

		glNamedBufferSubData(
			s_Renderer.instance_buf, 
			0,        
			instance_bytes, 
            s_Renderer.instance_data
		);

		glNamedBufferSubData(
			s_Renderer.light_instance_buf, 
			0,        
			light_instance_bytes, 
            s_Renderer.light_instance_data
		);

		BufferEntry buffer = s_Renderer.lods[5];
		


		s_Renderer.shader->Use();
		s_Renderer.shader->SetUniformMatrix4f("proj", cam.GetProjection());
		s_Renderer.shader->SetUniformMatrix4f("view", cam.GetView());
		s_Renderer.shader->SetUniform3f("lightPos", glm::vec3(0.0, 0.0, 0.0f));
		s_Renderer.shader->SetUniform3f("lightColor", glm::vec3(1.0f));
		s_Renderer.shader->SetUniform1i("lightCount", s_Renderer.light_instance_offset);
		//s_Renderer.shader->SetUniform1i("lod", lod);
		glDrawElementsInstancedBaseVertexBaseInstance(GL_TRIANGLES, buffer.index_count, GL_UNSIGNED_INT, (void*)buffer.index_byte_offset, s_Renderer.instance_offset, buffer.vertex_index, 0);

		s_Renderer.light_shader->Use();
		s_Renderer.light_shader->SetUniformMatrix4f("proj", cam.GetProjection());
		s_Renderer.light_shader->SetUniformMatrix4f("view", cam.GetView());
		s_Renderer.light_shader->SetUniform3f("lightPos", glm::vec3(0.0, 0.0, 0.0f));
		s_Renderer.light_shader->SetUniform3f("lightColor", glm::vec3(1.0f));
		s_Renderer.light_shader->SetUniform3f("camPos", cam.position);
		s_Renderer.light_shader->SetUniform1f("bias", cam.zoom);
		glDrawElementsInstancedBaseVertexBaseInstance(GL_TRIANGLES, buffer.index_count, GL_UNSIGNED_INT, (void*)buffer.index_byte_offset, s_Renderer.light_instance_offset, buffer.vertex_index, 0);
	
		s_Renderer.grain->SetUniform1f("time", (float)glfwGetTime());
		s_Renderer.finalpass->SetUniform1f("exposure", Global::GetPostProcessing().exposure);
		s_Renderer.finalpass->SetUniform1f("sqrexposure", Global::GetPostProcessing().sqrexposure);
		s_Renderer.finalpass->SetUniform1f("bloom", Global::GetPostProcessing().bloom);
		s_Renderer.finalpass->SetUniform1f("gamma", Global::GetPostProcessing().gamma);
		s_Renderer.postprocessing->Execute();
	}
	
	void Draw(Camera& cam, int lod, glm::vec3 position, float scale, glm::vec3 lightPos, glm::vec3 color)
	{
		BufferEntry buffer = s_Renderer.lods[lod];
		glm::mat4 model = glm::translate(glm::mat4(1.0), position);
		model = glm::scale(model, glm::vec3(scale));

		glBindVertexArray(s_Renderer.vao);
		s_Renderer.shader->SetUniformMatrix4f("model", model);
		s_Renderer.shader->SetUniformMatrix4f("proj", cam.GetProjection());
		s_Renderer.shader->SetUniformMatrix4f("view", cam.GetView());
		s_Renderer.shader->SetUniform3f("color", color);
		s_Renderer.shader->SetUniform3f("lightPos", lightPos);
		s_Renderer.shader->SetUniform3f("lightColor", glm::vec3(1.0f));
		s_Renderer.shader->SetUniform1i("lod", lod);
		glDrawElementsBaseVertex(GL_TRIANGLES, buffer.index_count, GL_UNSIGNED_INT, (void*)buffer.index_byte_offset, buffer.vertex_index);
		//glDrawElementsInstancedBaseVertexBaseInstance(GL_TRIANGLES, buffer.index_count, GL_UNSIGNED_INT, (void*)buffer.index_byte_offset, 1, buffer.vertex_index, 1);
		s_Count++;
	}

	unsigned int quadVAO = 0;
	unsigned int quadVBO;
	void renderQuad()
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		if (quadVAO == 0)
		{
			float quadVertices[] = {
				// positions        // texture Coords
				-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
				-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
				1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
				1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
			};
			// setup plane VAO
			glGenVertexArrays(1, &quadVAO);
			glGenBuffers(1, &quadVBO);
			glBindVertexArray(quadVAO);
			glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
		}
		glBindVertexArray(quadVAO);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		glBindVertexArray(0);
	}
}
