#include "rendering/icosphere.h"
#include "sim/Sim.h"
#include "window.h"
#include "ui/ui.h"
#include "ui/debug.h"
#include "settings.h"

#include <chrono>
#include <memory>

#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>
#include <imgui/imgui.h>
#include <imgui/misc/cpp/imgui_stdlib.h>

int main()
{
	const WindowSpecification spec = { 800, 800, "Ouroboros Simulator",
		false, false };

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

	while (window->isOpen())
	{
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

		IcoSphere::Draw(debug_view->GetLOD(), glm::vec3(0.0), 1.0);

		ui->OnUpdate();

		glfwSwapBuffers(window->GetNativeWindow());
		glfwPollEvents();
	}

	return 0;
}
