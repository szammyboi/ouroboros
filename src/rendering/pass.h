#pragma once

#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "shader.h"

struct RenderPass
{
    GLuint framebuffer;
    GLuint color_tex;
    GLuint alt_tex;
    GLuint depth_rbo;
    glm::vec2 resolution;
    RenderPass* previous = nullptr;

    RenderPass(glm::vec2 res)
    {
        glGenFramebuffers(1, &framebuffer);
		glGenTextures(1, &color_tex);
        glGenTextures(1, &alt_tex);
        glGenRenderbuffers(1, &depth_rbo);

        CreateBuffer(res);
    }

    void CreateBuffer(glm::vec2 res)
    {
        resolution = res;

        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
		glBindTexture(GL_TEXTURE_2D, color_tex);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, res.x , res.y, 0, GL_RGBA, GL_FLOAT, NULL);
        glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		// attach texture to framebuffer
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + 0, GL_TEXTURE_2D, color_tex, 0);

        glBindTexture(GL_TEXTURE_2D, alt_tex);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, res.x , res.y, 0, GL_RGBA, GL_FLOAT, NULL);
        glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		// attach texture to framebuffer
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + 1, GL_TEXTURE_2D, alt_tex, 0);

        GLenum attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
        glDrawBuffers(2, attachments);

        glBindRenderbuffer(GL_RENDERBUFFER, depth_rbo);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, res.x, res.y);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depth_rbo);
        
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glBindRenderbuffer(GL_RENDERBUFFER, 0);
    }

};

struct RenderPassInfo {
    Shader* shader;
    glm::vec2 scale;
};

struct RenderSequence
{
    unsigned int quadVAO = 0;
	unsigned int quadVBO;
    
    RenderPass mainImage;
    std::vector<RenderPass> passes;
    std::vector<RenderPassInfo> pass_info;

    RenderSequence(glm::vec2 resolution, std::initializer_list<RenderPassInfo> pass_list)
        : pass_info(pass_list), mainImage(resolution)
    {
        passes.reserve(pass_info.size());

        for (int i = 0; i < pass_info.size(); i++) {
            RenderPass& pass = passes.emplace_back(resolution * pass_info[i].scale);
            pass.previous = i == 0 ? &mainImage : &passes[i-1];
        }
    }

    void Resize(glm::vec2 resolution)
    {
        mainImage.CreateBuffer(resolution);
        for (int i = 0; i < passes.size(); i++)
            passes[i].CreateBuffer(resolution * pass_info[i].scale);
    }

    void SetupMainImage()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, mainImage.framebuffer);
		glViewport(0, 0, mainImage.resolution.x, mainImage.resolution.y);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void Execute()
    {
        glActiveTexture(GL_TEXTURE0 + 2);
        glBindTexture(GL_TEXTURE_2D, mainImage.alt_tex);
        glGenerateMipmap(GL_TEXTURE_2D);

        for (int i = 0; i < passes.size(); i++) {
            RenderPassInfo& info = pass_info[i];
            RenderPass& pass = passes[i];

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, pass.previous->color_tex);
            glGenerateMipmap(GL_TEXTURE_2D);
            //glBindFramebuffer(GL_FRAMEBUFFER, pass.framebuffer);
            if (i == passes.size() - 1)
                glBindFramebuffer(GL_FRAMEBUFFER, 0);
            else
                glBindFramebuffer(GL_FRAMEBUFFER, pass.framebuffer);
                
            glViewport(0, 0, pass.resolution.x, pass.resolution.y);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glActiveTexture(GL_TEXTURE0);

            glBindTexture(GL_TEXTURE_2D, pass.previous->color_tex);

            glActiveTexture(GL_TEXTURE0 + 1);
            glBindTexture(GL_TEXTURE_2D, mainImage.color_tex);

             glActiveTexture(GL_TEXTURE0 + 2);
            glBindTexture(GL_TEXTURE_2D, mainImage.alt_tex);

            info.shader->SetUniform2f("uResolution", pass.resolution);
            info.shader->SetUniform1i("previous", 0);
            info.shader->SetUniform1i("original", 1);
            info.shader->SetUniform1i("alt", 2);
            renderQuad();

            
        }
    }

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
};