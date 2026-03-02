#pragma once

#include "rendering/camera.h"

#include "global.h"

void Camera::Update()
{
    /*glm::vec3 f;
    f.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    f.y = sin(glm::radians(pitch));
    f.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

    front = glm::normalize(f);
    right = glm::normalize(glm::cross(front, WORLD_UP));
    up = glm::normalize(glm::cross(right, front));

    int fb_w, fb_h; 
    glfwGetFramebufferSize(Global::GetWindow()->GetNativeWindow(), &fb_w, &fb_h);
    proj = glm::perspective(glm::radians(45.0f), (float)fb_w / fb_h, 1.0f, 400.0f);
    
    */

    float cp = cos(glm::radians(pitch));
    float sp = sin(glm::radians(pitch));
    float cy = cos(glm::radians(yaw));
    float sy = sin(glm::radians(yaw));

    position = target + glm::vec3(
        zoom * cp * cy,
        zoom * sp,
        zoom * cp * sy
    );

    front = glm::normalize(target - position);
    right = glm::normalize(glm::cross(front, WORLD_UP));
    up = glm::normalize(glm::cross(right, front));

    int fb_w, fb_h;
    glfwGetFramebufferSize(Global::GetWindow()->GetNativeWindow(), &fb_w, &fb_h);
    proj = glm::perspective(glm::radians(45.0f), (float)fb_w / fb_h, 0.1f, 1000.0f);
}

void Camera::SetZoom(float z)
{
    zoom = z;
    zoom = std::min(45.0f, std::max(1.0f, zoom));
}

void Camera::Scroll(float offset)
{
    //position += front * offset * SCROLL_SPEED;
    zoom -= offset * SCROLL_SPEED;
    zoom  = std::max(0.5f, zoom);
}

void Camera::OnMouseButton(int button, int action)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        isDragging = (action == GLFW_PRESS);
    }
}

void Camera::OnMouseMove(float x, float y)
{
    if (isDragging) {
        float dx = x - lastMouseX;
        float dy = y - lastMouseY;

        yaw += dx * PAN_SPEED;
        pitch += dy * PAN_SPEED;
        pitch = glm::clamp(pitch, -89.0f, 89.0f);
    }

    lastMouseX = x;
    lastMouseY = y;
}

glm::mat4 Camera::GetView()
{
    return glm::lookAt(position, target, up);
    //return glm::lookAt(position, position + front, up);
}

glm::mat4 Camera::GetProjection()
{
    return proj;
}