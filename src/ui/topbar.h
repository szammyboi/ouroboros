#pragma once

#include "ui/view.h"

class TopBar : public View
{
public:
	TopBar(const Window& window)
	    : View(window) {}
	using View::View;

	void OnAttach() override;
	void OnDetach() override;
	void OnUpdate() override;
private:
    char m_Buffer1[100] = "\n";
	char m_Buffer2[100] = "\n";
	double m_LastTime;
	int m_nbFrames;
};
