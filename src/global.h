#pragma once

#include <memory>

#include "sim/Sim.h"
#include "rendering/camera.h"
#include "window.h"

struct Settings {
    struct Render {
        bool drawOutlines = true;
        bool drawOctree = true;
    } render;

    bool close = false;
};

class Global {
public:
    static Camera& GetCamera()
    { return Get().m_Camera; }

    static Sim& GetSim()
    { return Get().m_Sim; }

    static Settings& GetSettings()
    { return Get().m_Settings; }

    static std::shared_ptr<Window> GetWindow()
    { return Get().m_Window; }

    static void Initialize(const WindowSpecification& spec)
    {
        Global& global = Get();
        global.m_Window = std::make_shared<Window>(spec);
    }

    static Global& Get()
    {
        static Global global;
        return global;
    }
private:
    Global() = default;
private:
    Sim m_Sim;
    Camera m_Camera;
    std::shared_ptr<Window> m_Window;
    Settings m_Settings;
};