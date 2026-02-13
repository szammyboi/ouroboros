#pragma once

#include "ui/view.h"
#include "settings.h"

class UI : public View
{
public:
	using View::View;

	void OnAttach() override;
	void OnDetach() override;
	void OnUpdate() override;

private:
	RenderSettings m_RenderSettings;
	bool m_DemoWindow = false;
};
