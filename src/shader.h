#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>

namespace Ouroboros {
	class Shader
	{
	public:
		Shader(const std::string& vertex_path, const std::string& fragment_path);
		~Shader() = default;

		void Use() const;

		//void SetUniform1i(const std::string& uniform, int x);
		void SetUniformMatrix4f(const std::string& uniform, glm::mat4 matrix);
	private:
		unsigned m_Program;
		std::string m_VertexSource;
		std::string m_FragmentSource;
	};
}
