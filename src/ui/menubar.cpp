#include "ui/menubar.h"
#include "global.h"
#include "ui/fa.h"

#include <random>

#define NOMINMAX
#include <windows.h>

void MenuBar::OnAttach()
{
}

void MenuBar::OnDetach()
{
}

constexpr float PI = 3.14159265358979323846f;

static glm::vec3 random_unit_vec(std::mt19937& rng)
{
    std::uniform_real_distribution<float> u(0.0f, 1.0f);

    float theta = acos(1.0f - 2.0f * u(rng));
    float phi   = 2.0f * PI * u(rng);

    return {
        sin(theta) * cos(phi),
        sin(theta) * sin(phi),
        cos(theta)
    };
}

void add_plummer_bodies(Sim& sim, int n, float total_mass, float scale_radius, glm::vec3 center_offset)
{
    std::mt19937 rng(std::random_device{}());
    std::uniform_real_distribution<float> uniform(0.0f, 1.0f);
	std::uniform_real_distribution<float> temp_dist(1500.0f, 20000.0f);

    float body_mass = total_mass / float(n);

    std::vector<Body> temp;
    temp.reserve(n);

    glm::vec3 com_pos(0.0f);
    glm::vec3 com_vel(0.0f);

    for (int i = 0; i < n; i++)
    {
        float u = uniform(rng);
        float r = scale_radius / sqrt(pow(u, -2.0f / 3.0f) - 1.0f);
		float world_scale = 20.0f;
        glm::vec3 dir = random_unit_vec(rng);
        glm::vec3 pos = dir * (r * world_scale) + center_offset;

        float G = sim.config.G;

        float v_esc =
            sqrt(2.0f) *
            pow(r * r + scale_radius * scale_radius, -0.25f) *
            sqrt(G * total_mass);

        float v;
        do {
            v = uniform(rng) * v_esc;
        } while (uniform(rng) >
                 v * v *
                 pow(1.0f - (v * v) / (v_esc * v_esc), 3.5f) *
                 7.5f);

        glm::vec3 vel = random_unit_vec(rng) * v;

        Body b;
        b.mass = body_mass;
        b.radius = std::cbrt(body_mass) * 0.1f;
        b.loc = pos;
        b.vel = vel;
		b.temperature = temp_dist(rng);

        temp.push_back(b);

        com_pos += b.loc * b.mass;
        com_vel += b.vel * b.mass;
    }

    com_pos /= total_mass;
    com_vel /= total_mass;

    for (auto& b : temp)
    {
        b.loc -= com_pos;
        b.vel -= com_vel;
        sim.add_body(b);
    }
}

void MenuBar::OnUpdate()
{

	const int drag_threshold = 2;
	static bool dragging = false;
	static RECT initialWindowRect;
	static POINT initialMousePosition;
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, GetSystemMetrics(SM_CYFRAME)));


	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
	if (ImGui::BeginMainMenuBar())
	{

		if (dragging)
		{
			POINT mouseNew;
			GetCursorPos(&mouseNew);

			int dx = mouseNew.x - initialMousePosition.x;
			int dy = mouseNew.y - initialMousePosition.y;

			if (abs(dx) > drag_threshold || abs(dy) > drag_threshold)
			{
				SetWindowPos(m_Window.GetHWND(), NULL,
				    initialWindowRect.left + dx,
				    initialWindowRect.top + dy,
				    0, 0,
				    SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE);
			}
		}

		if (!dragging && ImGui::IsWindowHovered() && ImGui::IsMouseClicked(0))
		{
			dragging = true;

			GetCursorPos(&initialMousePosition);
			DwmGetWindowAttribute(m_Window.GetHWND(), DWMWA_EXTENDED_FRAME_BOUNDS, &initialWindowRect, sizeof(initialWindowRect));
		}

		if (!ImGui::IsMouseDown(0)) dragging = false;


		ImGui::PopStyleVar();

		ImGui::PopStyleVar();
		ImGui::SetCursorPosY(-GetSystemMetrics(SM_CYFRAME));


		ImGui::AlignTextToFramePadding();

		ImGui::PushStyleColor(ImGuiCol_Text, Catppuccin::red);
		if (ImGui::MenuItem(ICON_FA_XMARK)) {
			glfwSetWindowShouldClose(m_Window.GetNativeWindow(), GLFW_TRUE);
		}
		ImGui::PopStyleColor();

		ImGui::PushStyleColor(ImGuiCol_Text, Global::GetSim().isPlaying() ? Catppuccin::green : Catppuccin::text);
		if (ImGui::MenuItem(Global::GetSim().isPlaying() ? ICON_FA_PLAY : ICON_FA_PAUSE)) {
			Global::GetSim().Toggle();
		}
		ImGui::PopStyleColor();

		if (ImGui::BeginMenu(ICON_FA_GEAR))
		{
			if (ImGui::BeginMenu("Rendering"))
			{
				ImGui::PushItemFlag(ImGuiItemFlags_AutoClosePopups, false);
				if (ImGui::MenuItem("Draw Outlines", "", &Global::GetSettings().render.drawOutlines))
				{
					if (Global::GetSettings().render.drawOutlines)
						glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
					else
						glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
				}

				ImGui::MenuItem("Draw Octree", "", &Global::GetSettings().render.drawOctree);
				ImGui::MenuItem("Draw Planets", "", &Global::GetSettings().render.drawPlanets);
				ImGui::MenuItem("Draw Stars", "", &Global::GetSettings().render.drawStars);

				ImGui::PopItemFlag();
				ImGui::EndMenu();
			}

			if (ImGui::MenuItem("Close"))
			{
				glfwSetWindowShouldClose(m_Window.GetNativeWindow(), GLFW_TRUE);
			}

			if (ImGui::MenuItem("Spawn 8000 bodies")) {

				add_plummer_bodies(
					Global::GetSim(),
					8000,
					1.0f, 
					2.0f, 
					glm::vec3(0.0f)
				);
			}

			ImGui::EndMenu();
		}
		
				
		ImGui::EndMainMenuBar();
	}
}
