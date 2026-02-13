#include "ui/menubar.h"

#ifndef __APPLE__
	#define NOMINMAX
	#include <windows.h>
#endif

void MenuBar::OnAttach()
{
}

void MenuBar::OnDetach()
{
}

void MenuBar::OnUpdate()
{	
	const int drag_threshold = 2;
	static bool dragging = false;
	static RECT initialWindowRect;
	static POINT initialMousePosition;

	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
#ifndef __APPLE__
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, GetSystemMetrics(SM_CYFRAME)));
#endif
	if (ImGui::BeginMainMenuBar())
	{
#ifndef __APPLE__
		if (dragging)
		{	
			POINT mouseNew;
			GetCursorPos(&mouseNew);

			int dx = mouseNew.x - initialMousePosition.x;
			int dy = mouseNew.y - initialMousePosition.y;
			
			if (abs(dx) > drag_threshold || abs(dy) > drag_threshold) {
				SetWindowPos(m_Window.GetHWND(), NULL,
					initialWindowRect.left + dx,
					initialWindowRect.top + dy,
					0, 0,
					SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE
				);
			}
		}

		if (!dragging && ImGui::IsWindowHovered() && ImGui::IsMouseClicked(0)) {
			dragging = true;

			GetCursorPos(&initialMousePosition);
			DwmGetWindowAttribute(m_Window.GetHWND(), DWMWA_EXTENDED_FRAME_BOUNDS, &initialWindowRect, sizeof(initialWindowRect));
		}

		if (!ImGui::IsMouseDown(0)) dragging = false;
#endif

		ImGui::PopStyleVar();
#ifndef __APPLE__
		ImGui::PopStyleVar();
		ImGui::SetCursorPosY(-GetSystemMetrics(SM_CYFRAME));
#endif
		
		ImGui::AlignTextToFramePadding();
		if (ImGui::BeginMenu("Settings"))
		{
			if (ImGui::BeginMenu("Rendering"))
			{
				if (ImGui::MenuItem("Draw Outlines", "", &m_RenderSettings.drawOutlines))
				{
					if (m_RenderSettings.drawOutlines)
						glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
					else
						glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
				}
				ImGui::EndMenu();
			}

			if (ImGui::MenuItem("Close"))
			{
				glfwSetWindowShouldClose(m_Window.GetNativeWindow(), GLFW_TRUE);
			}

			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}
	//ImGui::PopStyleVar(2);
}
