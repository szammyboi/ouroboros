#pragma once

#include "ui/view.h"

class UI : public View
{
public:
	using View::View;

	void OnAttach() override;
	void OnDetach() override;
	void OnUpdate() override;

private:
	bool m_DemoWindow = false;
};
