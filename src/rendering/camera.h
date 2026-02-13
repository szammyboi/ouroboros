#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


struct Camera
{
    glm::vec3 position = glm::vec3(0.0, 0.0, 3.0f);
    glm::mat4 projection;
    glm::mat4 view;
};