#include "ui/ui.h"
#include "ui/menubar.h"
#include "ui/debug.h"
#include "ui/theme.h"
#include "ui/topbar.h"
#include "ui/fa.h"

void UI::OnAttach()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	ImFont* font = ImGui::GetIO().Fonts->AddFontFromFileTTF("assets/fonts/SF-Pro-Text-Regular.otf", 20.0f);
	font->Scale = 0.9;

	ImFontConfig icons_config;
	icons_config.MergeMode = true;
	icons_config.PixelSnapH = true;
	static const ImWchar icon_ranges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };
	font = ImGui::GetIO().Fonts->AddFontFromFileTTF("assets/fonts/solid.otf", 16.0f, &icons_config, icon_ranges);
	font->Scale = 0.9;

	const char* glsl_version = "#version 410";
	setup_catppuccin_mocha_theme();

	ImGui_ImplGlfw_InitForOpenGL(m_Window.GetNativeWindow(), true);
	ImGui_ImplOpenGL3_Init(glsl_version);

	AddView<MenuBar>();
	AddView<Debug>();
	AddView<TopBar>();
};

void UI::OnDetach()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

void UI::OnUpdate()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();

	ImGui::NewFrame();

	if (m_DemoWindow)
		ImGui::ShowDemoWindow();

	Slot();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
