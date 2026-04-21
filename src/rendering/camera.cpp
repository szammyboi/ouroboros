#pragma once

#include "rendering/camera.h"

#include "global.h"

void Camera::Update()
{
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
    proj = glm::perspective(glm::radians(45.0f), (float)fb_w / fb_h, 0.0001f, 1e15f);
}

void Camera::SetZoom(float z)
{
    zoom = z;
    zoom = std::min(45.0f, std::max(1.0f, zoom));
}

void Camera::Scroll(float offset)
{
    zoom *= exp(offset);
    zoom  = std::max(0.01f, zoom);
    zoom = std::min(zoom, 6.684582e+9f);
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

void Camera::GetViewSizeAU(float& width, float& height)
{
    int fb_w, fb_h;
    glfwGetFramebufferSize(Global::GetWindow()->GetNativeWindow(), &fb_w, &fb_h);

    float aspect = (float)fb_w / fb_h;
    float fov = glm::radians(45.0f);

    float d = zoom; // distance to target (AU)

    height = 2.0f * d * tan(fov * 0.5f);
    width  = height * aspect;
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