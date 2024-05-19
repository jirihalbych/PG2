#pragma once

#include "glm/glm.hpp"

struct DirectionalLight {
    glm::vec3 direction;

    glm::vec3 ambient = glm::vec3(0.0);
    glm::vec3 diffusion = glm::vec3(1.0);
    glm::vec3 specular = glm::vec3(1.0);
};