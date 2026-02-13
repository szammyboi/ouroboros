#pragma once

#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <string>

struct WindowSpecification
{
	unsigned width = 500;
	unsigned height = 500;
	std::string title = "Ouroboros Simulator";
	bool fullscreen = false;
	bool resizable = false;
};

class Window
{
public:
	Window(const WindowSpecification& spec);
	~Window();

	GLFWwindow* GetNativeWindow() const { return m_Window; }
	inline bool isOpen() { return !glfwWindowShouldClose(m_Window); }

private:
	void Initialize();

	static void WindowPositionCallback(GLFWwindow* window, int width, int height);
	static void FrameBufferSizeCallback(GLFWwindow* window, int x, int y);

private:
	GLFWwindow* m_Window;
	WindowSpecification m_Specification;
};
