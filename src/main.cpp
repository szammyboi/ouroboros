#include "window.h"
#include "icosphere.h"

#include <memory>

int main()
{
    const Ouroboros::WindowSpecification spec = {
        800, 500, "Ouroboros Simulator", false, false
    };

    std::unique_ptr<Ouroboros::Window> window = std::make_unique<Ouroboros::Window>(spec);


    auto icosphere = GenerateIcoSphere(0);

    GLuint vb, ib, vao;

    glGenVertexArrays(1, &vao);
    glCreateBuffers(1, &vb);
    glCreateBuffers(1, &ib);

    glNamedBufferStorage(vb, icosphere.first.size() * 3 * sizeof(float), icosphere.first.data(), GL_DYNAMIC_STORAGE_BIT);
    glNamedBufferStorage(ib, icosphere.second.size() * 3 * sizeof(int), icosphere.second.data(), GL_DYNAMIC_STORAGE_BIT);

    glVertexArrayVertexBuffer(vao, 0, vb, 0, sizeof(float) * 3);
    glVertexArrayElementBuffer(vao, ib);

    glVertexArrayAttribFormat(
        vao,
        0,
        3,
        GL_FLOAT,
        GL_FALSE,
        0
    );

    glVertexArrayAttribBinding(vao, 0, 0);
    



    while (window->isOpen())
    {
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glDrawElements(GL_TRIANGLES, icosphere.second.size(), GL_UNSIGNED_INT, (void*)0);

        glfwSwapBuffers(window->GetNativeWindow());
		glfwPollEvents();
    }

    return 0;
}

