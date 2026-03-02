#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

struct Camera
{
    static constexpr glm::vec3 WORLD_UP {0.0f, 1.0f, 0.0f};
    static constexpr float SCROLL_SPEED = 12.5f;
    static constexpr float PAN_SPEED = 1.5f;
    float zoom = 1.0f;
    float pitch = 0.0f;
    float yaw = -90.0f;

    glm::vec3 position = glm::vec3(0.0, 0.0, 3.0f);
    glm::vec3 front = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 target = glm::vec3(0.0f);
    glm::vec3 right;
    glm::mat4 proj;

    bool  isDragging = false;
    float lastMouseX = 0.0f;
    float lastMouseY = 0.0f;

    void Update();

    void SetZoom(float z);
    void Scroll(float offset);
    void OnMouseMove(float x, float y);
    void OnMouseButton(int button, int action);


    glm::mat4 GetView();

    glm::mat4 GetProjection();
};