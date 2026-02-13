#include "rendering/icosphere.h"
#include "sim/Sim.h"
#include "window.h"
#include "ui/ui.h"
#include "ui/debug.h"
#include "settings.h"
#include "rendering/camera.h"

#include <chrono>
#include <memory>

#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>
#include <imgui/imgui.h>
#include <imgui/misc/cpp/imgui_stdlib.h>

int main()
{
	const WindowSpecification spec = { 800, 800, "Ouroboros Simulator",
		false, false, true };

	Sim sim = Sim();
	std::unique_ptr<Window> window =
	    std::make_unique<Window>(spec);

	auto last = std::chrono::high_resolution_clock::now();
	double accumulator = 0.0;

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	int lod_level = 0;

	IcoSphere::InitRenderer();

	Shared<View> ui = View::CreateView<UI>(*window);

	bool polygonMode = true;

	auto debug_view = ui->GetView<Debug>();
	
	int fb_w, fb_h;
	Camera cam;

	while (window->isOpen())
	{
		glfwGetFramebufferSize(window->GetNativeWindow(), &fb_w, &fb_h);
		cam.view = glm::lookAt(cam.position,
		    glm::vec3(0.0f, 0.0f, 0.0f),
		    glm::vec3(0.0f, 1.0f, 0.0f));
		cam.projection = glm::perspective(glm::radians(45.0f), (float)fb_w / fb_h, 1.0f, 400.0f);

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

		IcoSphere::Draw(cam, debug_view->GetLOD(), glm::vec3(0.0), 1.0);

		ui->OnUpdate();

		glfwSwapBuffers(window->GetNativeWindow());
		glfwPollEvents();
	}

	return 0;
}
