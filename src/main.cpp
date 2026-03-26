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

	Body sun;
	sun.mass = 1.989e6f;
	sun.emission = glm::vec4(0.97f, 0.84f, 0.1f, 1.0f);
	sim.add_body(sun);

	auto add_planet = [&](float mass, float radius, float distance, float angle = 0.0f)
	{
		Body planet;
		planet.mass = mass;
		planet.loc = { distance * cos(angle), distance * sin(angle), 0.0f };
		float r = glm::length(planet.loc - sun.loc);
		float speed = sqrt(sim.config.G * sun.mass / r);
		planet.vel = { -speed * sin(angle), speed * cos(angle), 0.0f };
		sim.add_body(planet);
	};

	add_planet(sun.mass * 1.651e-7f, 0.38f,  0.39f);  // Mercury
	add_planet(sun.mass * 2.447e-6f, 0.95f,  0.72f);  // Venus
	add_planet(sun.mass * 3.003e-6f, 1.00f,  1.00f);  // Earth
	add_planet(sun.mass * 3.227e-7f, 0.53f,  1.52f);  // Mars
	add_planet(sun.mass * 9.545e-4f, 11.2f,  5.20f);  // Jupiter
	add_planet(sun.mass * 2.857e-4f, 9.45f,  9.58f);  // Saturn
	add_planet(sun.mass * 4.366e-5f, 4.01f, 19.20f);  // Uranus
	add_planet(sun.mass * 5.151e-5f, 3.88f, 30.05f);  // Neptune
		
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

		int i = 0;
		IcoSphere::StartBatch();
		for (Body& body : sim.Bodies) {
			glm::mat4 model = glm::translate(glm::mat4(1.0), body.loc);
			model = glm::scale(model, glm::vec3(0.1));

			float distance = glm::distance(Global::GetCamera().position, body.loc);
			int lodLevel = static_cast<int>(glm::floor(log2(distance / 5.0f + 1e-6f)));
			lodLevel = 5 - glm::clamp(lodLevel, 0, 5);
			
			if (body.emission.a != 0.0f)
				IcoSphere::SubmitLight(model, body.loc, body.emission);
			else
				IcoSphere::Submit(model, glm::vec4(1.0));
		}
		IcoSphere::EndBatch(Global::GetCamera());

		Octree::StartBatch();
		if (Global::GetSettings().render.drawOctree)
			Octree::Submit(sim.m_Tree);
		Octree::EndBatch(Global::GetCamera());
			
		ui->OnUpdate();

		glfwSwapBuffers(window->GetNativeWindow());
		glfwPollEvents();
	}
	return 0;
}
