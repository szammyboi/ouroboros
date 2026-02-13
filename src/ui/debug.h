#pragma once

#include "ui/view.h"

class Debug : public View
{
public:
	Debug(const Window& window)
	    : View(window) {}
	using View::View;

	void OnAttach() override;
	void OnDetach() override;
	void OnUpdate() override;

	int GetLOD()
	{
		return m_LOD;
	}

private:
	int m_LOD = 0;
};
