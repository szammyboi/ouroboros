#pragma once

#include "ui/view.h"
#include "settings.h"

class MenuBar : public View
{
public:
	MenuBar(const Window& window, RenderSettings& render_settings)
	    : View(window), m_RenderSettings(render_settings) {}
	using View::View;

	void OnAttach() override;
	void OnDetach() override;
	void OnUpdate() override;

private:
	RenderSettings& m_RenderSettings;
};
