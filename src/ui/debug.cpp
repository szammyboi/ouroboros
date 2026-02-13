#include "ui/debug.h"

void Debug::OnAttach()
{
}

void Debug::OnDetach()
{
}

void Debug::OnUpdate()
{
	static int location = 3;
	const float PAD = 10.0f;
	ImGuiIO& io = ImGui::GetIO();
	ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;

	const ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImVec2 work_pos = viewport->WorkPos;
	ImVec2 work_size = viewport->WorkSize;
	ImVec2 window_pos, window_pos_pivot;

	window_pos.x = work_pos.x + work_size.x - PAD;
	window_pos.y = work_pos.y + work_size.y - PAD;
	window_pos_pivot.x = 1.0f;
	window_pos_pivot.y = 1.0f;
	ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
	window_flags |= ImGuiWindowFlags_NoMove;

	if (ImGui::Begin("Debug View", NULL, window_flags))
	{
		if (ImGui::BeginTable("table2", 3))
		{
			for (int row = 0; row < 1; row++)
			{
				ImGui::TableNextRow();
				ImGui::TableNextColumn();
				ImGui::AlignTextToFramePadding();
				ImGui::Text("Subdivisions: ");
				ImGui::TableNextColumn();
				ImGui::SetNextItemWidth(100);
				if (ImGui::InputInt("### subdivisions", &m_LOD, 1, 1))
				{
					m_LOD = std::min(5, std::max(0, m_LOD));
				}
			}
			ImGui::EndTable();
		}

		ImGui::End();
	}
}
