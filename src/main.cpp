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

	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		Camera& cam = Global::GetCamera();
		auto win = Global::GetWindow();
		int fb_w, fb_h;
		glfwGetFramebufferSize(win->GetNativeWindow(), &fb_w, &fb_h);

		float ndcX = (2.0f * cam.lastMouseX) / fb_w - 1.0f;
		float ndcY = 1.0f - (2.0f * cam.lastMouseY) / fb_h;

		glm::vec4 rayEye = glm::inverse(cam.GetProjection()) * glm::vec4(ndcX, ndcY, -1.0f, 1.0f);
		rayEye = glm::vec4(rayEye.x, rayEye.y, -1.0f, 0.0f);
		glm::vec3 rayDir = glm::normalize(glm::vec3(glm::inverse(cam.GetView()) * rayEye));
		glm::vec3 rayOrig = cam.position;

		Sim& sim = Global::GetSim();
		int closestIndex = -1;
		float closestT = FLT_MAX;
		const float radius = 0.1f;

		for (int i = 0; i < (int)sim.Bodies.size(); i++) {
			glm::vec3 oc = rayOrig - sim.Bodies[i].loc;
			float b = glm::dot(oc, rayDir);
			float c = glm::dot(oc, oc) - radius * radius;
			float disc = b * b - c;
			if (disc < 0) continue;
			float t = -b - sqrt(disc);
			if (t > 0 && t < closestT) {
				closestT = t;
				closestIndex = i;
			}
		}
		
		if (closestIndex != -1 || Global::GetSettings().selectedBody == -1)
			Global::GetSettings().selectedBody = closestIndex;
	}

	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
		Global::GetSettings().selectedBody = -1;
	}
}

void CursorPosCallback(GLFWwindow*, double x, double y)
{
    Global::GetCamera().OnMouseMove((float)x, (float)y);
}

int main()
{
	const WindowSpecification spec = { 1920, 1080, "Ouroboros Simulator",
		false, false, true };

	Global::Initialize(spec);

	Sim& sim = Global::GetSim();
	std::shared_ptr<Window> window = Global::GetWindow();

	glfwSetScrollCallback(window->GetNativeWindow(),      ScrollCallback);
	glfwSetMouseButtonCallback(window->GetNativeWindow(), MouseButtonCallback);
	glfwSetCursorPosCallback(window->GetNativeWindow(),   CursorPosCallback);

	auto last = std::chrono::high_resolution_clock::now();
	double accumulator = 0.0;

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	int lod_level = 0;

	IcoSphere::InitRenderer({1920, 1080});
	Octree::InitRenderer();

	Shared<View> ui = View::CreateView<UI>(*window);

	bool polygonMode = true;

	auto debug_view = ui->GetView<Debug>();

	Body sun;
	sun.mass = 1.0f;
	sun.emission = glm::vec4(0.97f, 0.84f, 0.1f, 1.0f);
	sun.radius = 0.0046;
	sun.temperature = 5800;
	sim.add_body(sun);

	Body earth;
	earth.mass = 3.003e-6;
	earth.loc = glm::vec3(1.0, 0.0, 0.0);
	earth.vel = glm::vec3(0.0, 1.0, 0.0);
	earth.radius = 4.26352e-5;
	sim.add_body(earth);

	Body mercury;
	mercury.mass = 1.660e-7;
	mercury.loc = glm::vec3(0.387, 0.0, 0.0);
	mercury.vel = glm::vec3(0.0, 1.607, 0.0);
	mercury.radius = 1.631e-5;
	sim.add_body(mercury);

	Body venus;
	venus.mass = 2.447e-6;
	venus.loc = glm::vec3(0.723, 0.0, 0.0);
	venus.vel = glm::vec3(0.0, 1.174, 0.0);
	venus.radius = 4.045e-5;
	sim.add_body(venus);

	Body mars;
	mars.mass = 3.213e-7;
	mars.loc = glm::vec3(1.524, 0.0, 0.0);
	mars.vel = glm::vec3(0.0, 0.808, 0.0);
	mars.radius = 2.265e-5;
	sim.add_body(mars);

	Body jupiter;
	jupiter.mass = 9.545e-4;
	jupiter.loc = glm::vec3(5.203, 0.0, 0.0);
	jupiter.vel = glm::vec3(0.0, 0.439, 0.0);
	jupiter.radius = 4.778e-4;
	sim.add_body(jupiter);

	Body saturn;
	saturn.mass = 2.857e-4;
	saturn.loc = glm::vec3(9.537, 0.0, 0.0);
	saturn.vel = glm::vec3(0.0, 0.325, 0.0);
	saturn.radius = 4.027e-4;
	sim.add_body(saturn);

	Body uranus;
	uranus.mass = 4.365e-5;
	uranus.loc = glm::vec3(19.191, 0.0, 0.0);
	uranus.vel = glm::vec3(0.0, 0.228, 0.0);
	uranus.radius = 1.708e-4;
	sim.add_body(uranus);

	Body neptune;
	neptune.mass = 5.149e-5;
	neptune.loc = glm::vec3(30.068, 0.0, 0.0);
	neptune.vel = glm::vec3(0.0, 0.182, 0.0);
	neptune.radius = 1.655e-4;
	sim.add_body(neptune);

	
		
	while (window->isOpen())
	{
		Sim& sim = Global::GetSim();
		int sel = Global::GetSettings().selectedBody;
		//Global::GetCamera().Update();
		
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

		if(sel >= 0 && sel < (int)sim.Bodies.size())
			Global::GetCamera().target = sim.Bodies[sel].loc;
		Global::GetCamera().Update();

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		int i = 0;
		int selectedBody = Global::GetSettings().selectedBody;
		IcoSphere::StartBatch();
		for (Body& body : sim.Bodies) {
			glm::mat4 model = glm::translate(glm::mat4(1.0), body.loc);
			model = glm::scale(model, glm::vec3(0.046));

			float distance = glm::distance(Global::GetCamera().position, body.loc);
			int lodLevel = static_cast<int>(glm::floor(log2(distance / 5.0f + 1e-6f)));
			lodLevel = 5 - glm::clamp(lodLevel, 0, 5);

			if (body.temperature != 0.0f)
				IcoSphere::SubmitLight(model, body.loc, glm::vec4(body.temperature, glm::vec3(0.0)));
			else
				IcoSphere::Submit(model, glm::vec4(1.0));
			i++;
		}
		IcoSphere::EndBatch(Global::GetCamera());
		
		glClear(GL_DEPTH_BUFFER_BIT);

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
