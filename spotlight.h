#pragma once
#include "glm/glm.hpp"

struct Spotlight {
    glm::vec3 position;
    glm::vec3 direction;
    float inner = glm::cos(glm::radians(30.0));
    float outer = glm::cos(glm::radians(35.0));

    float constant = 1.0f;
    float linear = 0.14f;
    float quadratic = 0.07f;

    glm::vec3 ambient = glm::vec3{ 0.0f };
    glm::vec3 diffusion = glm::vec3{ 10.0f };
    glm::vec3 specular = glm::vec3{ 1.0f };
};