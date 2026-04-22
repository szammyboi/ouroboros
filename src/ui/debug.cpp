#include "ui/debug.h"
#include "global.h"
#include <format>

void Debug::OnAttach()
{
}

void Debug::OnDetach()
{
}

std::string CalculateUnits(float seconds)
{
	if (seconds > 3.154e+7) {
		return std::format("{:.2f} year(s)", seconds / 3.154e+7);
	}
	if (seconds > 86400) {
		return std::format("{:.2f} day(s)", seconds / 86400);
	}

	if (seconds > 3600) {
		return std::format("{:.2f} hours(s)", seconds / 3600);
	}

	if (seconds > 60) {
		return std::format("{:.2f} minute(s)", seconds / 60);
	}
	return std::format("{:.2f} second(s)", seconds);
}

std::string CalculateUnitsDistance(float distance)
{
	if (distance > 206265) {
		return std::format("{:.2f} PC", distance / 206265);
	}

	if (distance > 63241.1) {
		return std::format("{:.2f} LY", distance / 63241.1);
	}
	return std::format("{:.2f} AU", distance);
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
			ImGui::TableNextRow();
				ImGui::TableNextColumn();
				ImGui::AlignTextToFramePadding();
				ImGui::Text("Time Multiplier: ");
				ImGui::TableNextColumn();
				ImGui::SetNextItemWidth(100);
				ImGui::SliderFloat("### ttg", &Global::GetSim().config.timeMultiplier, 0.05, 10.0f, "%.7f");
			
			ImGui::TableNextRow();
				ImGui::TableNextColumn();
				ImGui::AlignTextToFramePadding();
				ImGui::Text("1 Second =  ");
				ImGui::TableNextColumn();
				ImGui::SetNextItemWidth(100);
				ImGui::Text(CalculateUnits(timescale_base * Global::GetSim().config.timeMultiplier).c_str());

			float width, height;
			Global::GetCamera().GetViewSizeAU(width, height);

			ImGui::TableNextRow();
				ImGui::TableNextColumn();
				ImGui::AlignTextToFramePadding();
				ImGui::Text("Distance: ");
				ImGui::TableNextColumn();
				ImGui::SetNextItemWidth(100);
				ImGui::Text(CalculateUnitsDistance(glm::distance(Global::GetCamera().position, Global::GetSettings().selectedBody != -1 ? Global::GetSim().Bodies[Global::GetSettings().selectedBody].loc : glm::vec3(0.0)) ).c_str());

			ImGui::TableNextRow();
				ImGui::TableNextColumn();
				ImGui::AlignTextToFramePadding();
				ImGui::Text("Frame Width: ");
				ImGui::TableNextColumn();
				ImGui::SetNextItemWidth(100);
				ImGui::Text(CalculateUnitsDistance(width).c_str());

			ImGui::TableNextRow();
				ImGui::TableNextColumn();
				ImGui::AlignTextToFramePadding();
				ImGui::Text("Frame Height: ");
				ImGui::TableNextColumn();
				ImGui::SetNextItemWidth(100);
				ImGui::Text(CalculateUnitsDistance(height).c_str());


			ImGui::TableNextRow();
				ImGui::TableNextColumn();
				ImGui::AlignTextToFramePadding();
				ImGui::Text("Gravity: ");
				ImGui::TableNextColumn();
				ImGui::SetNextItemWidth(100);
				ImGui::SliderFloat("### g", &Global::GetSim().config.G, 0.0, 10.0f, "%.7f");
			ImGui::EndTable();
		}

		ImGui::End();
	}
}
