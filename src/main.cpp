#include "window.h"
#include "sim/Sim.h"
#include <memory>
#include <chrono>

int main()
{
    const Ouroboros::WindowSpecification spec = {
        800, 500, "Ouroboros Simulator", false, false
    };

    Ouroboros::Sim sim = Ouroboros::Sim();
    std::unique_ptr<Ouroboros::Window> window = std::make_unique<Ouroboros::Window>(spec);

    auto last = std::chrono::high_resolution_clock::now();
    double accumulator = 0.0;
    while (window->isOpen())
    {
        auto now = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> delta = now - last;
        last = now;
        
        double frameTime = delta.count();
        accumulator += frameTime; 

        while (accumulator >= sim.config.simDt) {
            sim.step(sim.config.simDt);
            accumulator -= sim.config.simDt;
        }

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glfwSwapBuffers(window->GetNativeWindow());
		glfwPollEvents();
    }

    return 0;
}

