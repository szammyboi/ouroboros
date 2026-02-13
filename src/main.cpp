#include "rendering/icosphere.h"
#include "sim/Sim.h"
#include "window.h"
#include "theme.h"

#include <chrono>
#include <memory>

#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>
#include <imgui/imgui.h>
#include <imgui/misc/cpp/imgui_stdlib.h>

int main()
{
	const Ouroboros::WindowSpecification spec = { 800, 800, "Ouroboros Simulator",
		false, false };

	Ouroboros::Sim sim = Ouroboros::Sim();
	std::unique_ptr<Ouroboros::Window> window =
	    std::make_unique<Ouroboros::Window>(spec);

	auto last = std::chrono::high_resolution_clock::now();
	double accumulator = 0.0;

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	ImFont* font = ImGui::GetIO().Fonts->AddFontFromFileTTF(
	    "assets/fonts/SF-Pro-Text-Regular.otf");
	font->Scale = 0.9;

	const char* glsl_version = "#version 410";
	setup_catppuccin_mocha_theme();

	ImGui_ImplGlfw_InitForOpenGL(window->GetNativeWindow(), true);
	ImGui_ImplOpenGL3_Init(glsl_version);

	int lod_level = 0;

	IcoSphere::InitRenderer();

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

		IcoSphere::Draw(lod_level, glm::vec3(0.0), 1.0);

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		ImGui::SliderInt("Subdivisions", &lod_level, 0, 5);

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(window->GetNativeWindow());
		glfwPollEvents();
	}

	return 0;
}
