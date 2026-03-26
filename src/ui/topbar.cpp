#include "ui/topbar.h"
#include "fa.h"

void TopBar::OnAttach()
{
}

void TopBar::OnDetach()
{
}

void TopBar::OnUpdate()
{
    float frametime = 0.0;
    double currentTime = glfwGetTime();
    m_nbFrames++;

    if (currentTime - m_LastTime >= 1.0){
        frametime = 1000.0/double(m_nbFrames);
        snprintf(m_Buffer1, sizeof(m_Buffer1), "%f ms\n", 1000.0/double(m_nbFrames));
        snprintf(m_Buffer2, sizeof(m_Buffer2), "%.0f fps\n", double(m_nbFrames));
        m_nbFrames = 0;
        m_LastTime += 1.0;
    }

    ImVec4 color = {1.0, 0.0, 0.0, 1.0};

    if (frametime <= 6.94)
            color = Catppuccin::green;
    else if (frametime <= 16.67)
            color = Catppuccin::red;


	static int location = 3;
	const float PAD = 10.0f;
	ImGuiIO& io = ImGui::GetIO();
	ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;

	const ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImVec2 work_pos = viewport->WorkPos;
	ImVec2 work_size = viewport->WorkSize;
	ImVec2 window_pos, window_pos_pivot;

	window_pos.x = work_pos.x + work_size.x - PAD;
	window_pos.y = work_pos.y + PAD;
	window_pos_pivot.x = 1.0f;
	window_pos_pivot.y = 0.0f;
	ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
	window_flags |= ImGuiWindowFlags_NoMove;

	if (ImGui::Begin("Top Bar", NULL, window_flags))
	{
        ImGui::TextColored(color, m_Buffer1);
        ImGui::TextColored(color, m_Buffer2);
		ImGui::End();
	}
}
