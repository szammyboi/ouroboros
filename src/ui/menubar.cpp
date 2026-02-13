#include "ui/menubar.h"

void MenuBar::OnAttach()
{
}

void MenuBar::OnDetach()
{
}

void MenuBar::OnUpdate()
{
	if (ImGui::BeginMainMenuBar())
	{
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

			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}
}
