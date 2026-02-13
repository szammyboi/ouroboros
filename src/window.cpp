#include "window.h"

#include "GLFW/glfw3.h"
#include <iostream>

Window::Window(const WindowSpecification& spec) : m_Specification(spec)
{
	Initialize();
}

Window::~Window()
{
	glfwDestroyWindow(m_Window);
	glfwTerminate();
}

void Window::FrameBufferSizeCallback(GLFWwindow* window, int width,
    int height)
{
	glViewport(0, 0, width, height);
}

void Window::WindowPositionCallback(GLFWwindow* window, int width, int height)
{
}

void Window::Initialize()
{
	glfwInit();

#ifndef __APPLE__
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
#else
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	// glfwWindowHint(GLFW_COCOA_RETINA_FRAMEBUFFER, GLFW_FALSE);
#endif
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWmonitor* monitor = glfwGetPrimaryMonitor();

	if (m_Specification.fullscreen)
	{
		const GLFWvidmode* mode = glfwGetVideoMode(monitor);
		glfwWindowHint(GLFW_RED_BITS, mode->redBits);
		glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
		glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
		glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);

		m_Specification.width = mode->width;
		m_Specification.height = mode->height;
	}

	if (m_Specification.resizable == false)
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	m_Window =
	    glfwCreateWindow(m_Specification.width, m_Specification.height,
	        m_Specification.title.c_str(),
	        m_Specification.fullscreen ? monitor : nullptr, nullptr);

	if (m_Window == nullptr)
	{
		return;
	}

	glfwMakeContextCurrent(m_Window);

	int version = gladLoadGL(glfwGetProcAddress);
	if (version == 0)
	{
		return;
	}

	int fb_w, fb_h;
	glfwGetFramebufferSize(m_Window, &fb_w, &fb_h);
	glViewport(0, 0, fb_w, fb_h);
	//		glViewport(0, 0, m_Specification.width, m_Specification.height);

	glfwSetFramebufferSizeCallback(m_Window, FrameBufferSizeCallback);
	glfwSetWindowPosCallback(m_Window, WindowPositionCallback);
}
