#pragma once

#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <string>

#ifndef __APPLE__
	#define NOMINMAX
	#define GLFW_EXPOSE_NATIVE_WIN32
	#include <glfw/glfw3native.h>
	#include <dwmapi.h>
	#pragma comment(lib, "dwmapi.lib")
#endif

struct WindowSpecification
{
	unsigned width = 500;
	unsigned height = 500;
	std::string title = "Ouroboros Simulator";
	bool fullscreen = false;
	bool resizable = true;
	bool decorated = true;
};

class Window
{
public:
	Window(const WindowSpecification& spec);
	~Window();

	GLFWwindow* GetNativeWindow() const { return m_Window; }
	inline bool isOpen() { return !glfwWindowShouldClose(m_Window); }

	#ifndef __APPLE__
		HWND GetHWND() const { return m_HWND; }
	#endif

private:
	void Initialize();

	static void WindowPositionCallback(GLFWwindow* window, int width, int height);
	static void FrameBufferSizeCallback(GLFWwindow* window, int x, int y);

private:
	GLFWwindow* m_Window;
	WindowSpecification m_Specification;
	#ifndef __APPLE__
		HWND m_HWND;
	#endif
};
