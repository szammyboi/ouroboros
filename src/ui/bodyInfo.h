#pragma once
#include "ui/view.h"

class BodyInfo : public View {
    public:
        BodyInfo(const Window& window): View(window){}
        using View::View;

        void OnAttach() override {}
        void OnDetach() override {}
        void OnUpdate() override;
};