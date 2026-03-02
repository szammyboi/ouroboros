#include "rendering/icosphere.h"
#include "rendering/octree.h"
#include "sim/Sim.h"
#include "window.h"
#include "ui/ui.h"
#include "ui/debug.h"
#include "rendering/camera.h"

#include "global.h"

#include <chrono>
#include <memory>
	#include <random>

#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>
#include <imgui/imgui.h>
#include <imgui/misc/cpp/imgui_stdlib.h>

void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
	Global::GetCamera().Scroll((float)yoffset);
}

void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
}

void MouseButtonCallback(GLFWwindow*, int button, int action, int)
{
	ImGuiIO& io = ImGui::GetIO();

	if (io.WantCaptureMouse)
		return;
    Global::GetCamera().OnMouseButton(button, action);
}

void CursorPosCallback(GLFWwindow*, double x, double y)
{
    Global::GetCamera().OnMouseMove((float)x, (float)y);
}

int main()
{
	const WindowSpecification spec = { 800, 800, "Ouroboros Simulator",
		false, false, true };

	Global::Initialize(spec);

	Sim& sim = Global::GetSim();
	std::shared_ptr<Window> window = Global::GetWindow();

	glfwSetScrollCallback(window->GetNativeWindow(),      ScrollCallback);
	glfwSetMouseButtonCallback(window->GetNativeWindow(), MouseButtonCallback);
	glfwSetCursorPosCallback(window->GetNativeWindow(),   CursorPosCallback);

	auto last = std::chrono::high_resolution_clock::now();
	double accumulator = 0.0;

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	int lod_level = 0;

	IcoSphere::InitRenderer();
	Octree::InitRenderer();

	Shared<View> ui = View::CreateView<UI>(*window);

	bool polygonMode = true;

	auto debug_view = ui->GetView<Debug>();
		
	while (window->isOpen())
	{
		Global::GetCamera().Update();
		auto now = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double> delta = now - last;
		last = now;

		double frameTime = delta.count();
		accumulator += frameTime;

		while (accumulator >= sim.config.simDt)
		{
			sim.step(sim.config.simDt);
			accumulator -= sim.config.simDt;
		}

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		IcoSphere::Draw(Global::GetCamera(), debug_view->GetLOD(), glm::vec3(0.0), 1.0);
		
		if (Global::GetSettings().render.drawOctree)
			Octree::Draw(Global::GetCamera(), glm::vec3(0.0f), glm::vec3(2.0f), 1);

		ui->OnUpdate();

		glfwSwapBuffers(window->GetNativeWindow());
		glfwPollEvents();
	}
	return 0;
}
