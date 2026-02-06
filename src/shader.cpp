#include <fstream>
#include <iostream>

#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include "shader.h"

bool VerifyShader(unsigned handle, const std::string& file)
{
	int success;
	char infoLog[512];
	glGetShaderiv(handle, GL_COMPILE_STATUS, &success);

	if (success) return true;

	glGetShaderInfoLog(handle, 512, NULL, infoLog);

	std::cout << infoLog << std::endl;
	return false;
}

bool VerifyShaderProgram(unsigned handle, const std::string& vertex_path, const std::string& fragment_path)
{
	int success;
	char infoLog[512];
	glGetProgramiv(handle, GL_LINK_STATUS, &success);

	if (success) return true;
	glGetProgramInfoLog(handle, 512, NULL, infoLog);
	std::cout << infoLog << std::endl;
	return false;
}

std::string ReadFromFile(const std::string& path)
{
	std::string res;
	std::ifstream in(path, std::ios::in | std::ios::binary);
	if (!in) {
		std::cout << "Could Not Open Shader File" << std::endl;
		return res;
	}

	in.seekg(0, std::ios::end);
	size_t size = in.tellg();
	if (size == -1) return res;

	res.resize(size);
	in.seekg(0, std::ios::beg);
	in.read(&res[0], size);

	return res;
}

namespace Ouroboros {
	Shader::Shader(const std::string& vertex_path, const std::string& fragment_path)
	{
		m_Program = glCreateProgram();

		m_VertexSource = ReadFromFile(vertex_path);
		m_FragmentSource = ReadFromFile(fragment_path);

		unsigned vertex = glCreateShader(GL_VERTEX_SHADER);

		unsigned fragment = glCreateShader(GL_FRAGMENT_SHADER);

		const char* v_ref = m_VertexSource.c_str();
		const char* f_ref = m_FragmentSource.c_str();

		glShaderSource(vertex, 1, &v_ref, NULL);
		glShaderSource(fragment, 1, &f_ref, NULL);

		glCompileShader(vertex);
		VerifyShader(vertex, vertex_path);
		
		glCompileShader(fragment);
		VerifyShader(fragment, fragment_path);

		glAttachShader(m_Program, vertex);
		glAttachShader(m_Program, fragment);
		glLinkProgram(m_Program);

		VerifyShaderProgram(m_Program, vertex_path, fragment_path);

		glDeleteShader(vertex);
		glDeleteShader(fragment);
	}

	void Shader::Use() const
	{
		glUseProgram(m_Program);
	}

	void Shader::SetUniformMatrix4f(const std::string& uniform, glm::mat4 matrix)
	{
		Use();

		int location = glGetUniformLocation(m_Program, uniform.c_str());
		glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
	}
}
