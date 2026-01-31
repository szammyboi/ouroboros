#include "window.h"
#include <memory>

int main()
{
    const Ouroboros::WindowSpecification spec = {
        800, 500, "Ouroboros Simulator", false, false
    };

    std::unique_ptr<Ouroboros::Window> window = std::make_unique<Ouroboros::Window>(spec);

    while (window->isOpen())
    {
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glfwSwapBuffers(window->GetNativeWindow());
		glfwPollEvents();
    }

    return 0;
}

