#include "window.h"
#include "shader.h"
#include "geometry/icosphere.h"

#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>
#include <imgui/misc/cpp/imgui_stdlib.h>
#include "sim/Sim.h"
#include <memory>
#include <chrono>

struct InstanceData {
    glm::mat4 modelTransform;
    glm::vec3 position;
    float lod;
};

struct OffsetData {
    int vertex_index;
    int index_offset;
    int vertex_count;
    int index_count;
};

int main()
{
    const Ouroboros::WindowSpecification spec = {
        1200, 1200, "Ouroboros Simulator", false, false
    };

    Ouroboros::Sim sim = Ouroboros::Sim();
    std::unique_ptr<Ouroboros::Window> window = std::make_unique<Ouroboros::Window>(spec);


    std::vector<float> ico_vertices;
    std::vector<InstanceData> instance_data;
    std::vector<uint32_t> ico_indices;

    glm::mat4 ff = glm::mat4(1.0);
    ff = glm::translate(ff, glm::vec3(0.0, 0.0, -3.0));
    
    int grid_scale = 3;

    float starter_delta = -3.0 * grid_scale;
    for (int i = 0; i <= grid_scale; i++)
    {
        for (int j = 0; j < grid_scale; j++)
        {
            glm::mat4 initial_transform(1.0f);
            glm::vec3 initial_pos((starter_delta/2)+3*i, (starter_delta/2)+3*j, -15.4);
            initial_transform = glm::translate(initial_transform, initial_pos);

            InstanceData data;
            data.modelTransform = initial_transform;
            data.lod = (i+j)%5;
            data.position = initial_pos;

            instance_data.push_back(data);
        }
    }

    std::vector<float> all_vertices;
    std::vector<uint32_t> all_indices;
    std::vector<OffsetData> all_offsets;

    int vertex_index_offset = 0;
    int index_byte_offset = 0;
    for (int i = 0; i <= 5; i++)
    {
        OffsetData offset;
        offset.vertex_index = vertex_index_offset;
        offset.index_offset = index_byte_offset;

        const auto [vertices, indices] = GenerateIcoSphere(i);

        offset.vertex_count = vertices.size() / 3;
        offset.index_count = indices.size();

        all_vertices.insert(all_vertices.end(), vertices.begin(), vertices.end());
        all_indices.insert(all_indices.end(), indices.begin(), indices.end());
        all_offsets.push_back(offset);

        vertex_index_offset += offset.vertex_count;
        index_byte_offset += indices.size() * sizeof(uint32_t);
    }


    int subdivision_count = 5;
    auto icosphere = GenerateIcoSphere(subdivision_count);
    ico_vertices = icosphere.first;
    ico_indices = icosphere.second;

    GLuint vb, ib, vao, instance_buffer;

    glCreateVertexArrays(1, &vao);
    glCreateBuffers(1, &vb);
    glCreateBuffers(1, &ib);
    glCreateBuffers(1, &instance_buffer);

    glNamedBufferStorage(vb, all_vertices.size() * sizeof(float), all_vertices.data(), GL_DYNAMIC_STORAGE_BIT);
    glNamedBufferStorage(ib, all_indices.size() * sizeof(uint32_t), all_indices.data(), GL_DYNAMIC_STORAGE_BIT);

    glNamedBufferStorage(instance_buffer, instance_data.size() * sizeof(InstanceData), instance_data.data(), GL_DYNAMIC_STORAGE_BIT);

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
    glEnableVertexArrayAttrib(vao, 0);

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, instance_buffer);
 
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    Ouroboros::Shader shader("src/shaders/triangle.vert", "src/shaders/triangle.frag");
    
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS); 
    
    

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImFont* font = ImGui::GetIO().Fonts->AddFontFromFileTTF("assets/fonts/SF-Pro-Text-Regular.otf");
    font->Scale = 0.9;

    const char* glsl_version = "#version 460";
    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window->GetNativeWindow(), true);
    ImGui_ImplOpenGL3_Init(glsl_version);
    
    bool backface_culling = false;
    glm::vec3 position = glm::vec3(0.0, 0.0, -15.4);
    glm::mat4 perspective = glm::perspective(glm::radians(45.0f), 1200.0f / 1200.0f, 1.0f, 100.0f);

    // fps counter buffers and data
    char m_Buffer1[100] = "\n";
    char m_Buffer2[100] = "\n";
    double m_LastTime;
    int m_nbFrames;

    auto last = std::chrono::high_resolution_clock::now();
    double accumulator = 0.0;

    while (window->isOpen())
    {  
        float frametime = 0.0;
        double currentTime = glfwGetTime();
        m_nbFrames++;

        if ( currentTime - m_LastTime >= 1.0 ){
                frametime = 1000.0/double(m_nbFrames);
            snprintf(m_Buffer1, sizeof(m_Buffer1), "%f ms\n", 1000.0/double(m_nbFrames));
            snprintf(m_Buffer2, sizeof(m_Buffer2), "%.0f fps\n", double(m_nbFrames));
            m_nbFrames = 0;
            m_LastTime += 1.0;
        }
        
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

        glNamedBufferSubData(instance_buffer, 0, instance_data.size() * sizeof(InstanceData), instance_data.data());

        
        glBindVertexArray(vao);
        shader.SetUniformMatrix4f("perspective", perspective);

        int i = 0;
        for (auto& instance : instance_data)
        {
            instance.modelTransform = glm::translate(glm::mat4(1.0f), instance.position);
            instance.modelTransform = glm::rotate(instance.modelTransform, (float)glfwGetTime(), glm::vec3(0.45f, 0.45f, 0.0f));
            OffsetData lod = all_offsets[instance.lod];
            // this is overkill but will likely be used later
            glDrawElementsInstancedBaseVertexBaseInstance(GL_TRIANGLES, lod.index_count, GL_UNSIGNED_INT, (void*)lod.index_offset, 1, lod.vertex_index, i++);
        }

        ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();

		ImGui::NewFrame();

        ImVec4 color = {1.0, 0.0, 0.0, 1.0};

        if (frametime <= 6.94)
                color = {0.0, 1.0, 0.0, 1.0};
        else if (frametime <= 16.67)
                color = {0.0, 1.0, 1.0, 1.0};

        ImGui::TextColored(color, m_Buffer1);
        ImGui::TextColored(color, m_Buffer2);

        //ImGui::SliderInt("Sphere Subdivisions", &grid_scale, 0, 20);

        if (ImGui::SliderFloat3("Position", glm::value_ptr(position), -40.0f, 0.0f))
        {
            for (auto& instance : instance_data)
            {
                instance.position.z = position.z;
                instance.modelTransform = glm::translate(glm::mat4(1.0f), instance.position);
                instance.modelTransform = glm::rotate(instance.modelTransform, (float)glfwGetTime(), glm::vec3(0.45f, 0.45f, 0.0f));
            }
        }

        if (ImGui::Checkbox("Backface Culling", &backface_culling)) {
            if (backface_culling)
            {
                glEnable(GL_CULL_FACE);
                glCullFace(GL_BACK);
            } else 
            {
                glDisable(GL_CULL_FACE);
            }
            
        }

        ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window->GetNativeWindow());
		glfwPollEvents();
    }

    return 0;
}

