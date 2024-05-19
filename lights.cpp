#include "directional_light.h"
#include "ambient_light.h"
#include "spotlight.h"

void App::addDirectional(DirectionalLight &light){
    directionalLights.push_back(&light);
}

void App::addAmbient(AmbientLight &light){
    ambientLights.push_back(&light);
}

void App::addSpotlight(Spotlight &light){
    spotlights.push_back(&light);
}

void App::renderLights(ShaderProgram program){
    int i =0;
    for (DirectionalLight* light : directionalLights){
        program.setUniform("directionalLights["+std::to_string(i)+"].direction", light->direction);
        program.setUniform("directionalLights["+std::to_string(i)+"].ambient", light->ambient);
        program.setUniform("directionalLights["+std::to_string(i)+"].diffuse", light->diffusion);
        program.setUniform("directionalLights["+std::to_string(i)+"].specular", light->specular);
        i++;
    }
    i = 0;
    for (AmbientLight* light : ambientLights){
        program.setUniform("ambientLights["+std::to_string(i)+"].color", light->color);
        program.setUniform("ambientLights["+std::to_string(i)+"].intensity", light->intensity);
        i++;
    }
    i = 0;
    for (Spotlight* light : spotlights){
        program.setUniform("spotlights["+std::to_string(i)+"].position", light->position);
        program.setUniform("spotlights["+std::to_string(i)+"].direction", light->direction);
        program.setUniform("spotlights["+std::to_string(i)+"].inner", light->inner);
        program.setUniform("spotlights["+std::to_string(i)+"].outer", light->outer);
        program.setUniform("spotlights["+std::to_string(i)+"].constant", light->constant);
        program.setUniform("spotlights["+std::to_string(i)+"].linear", light->linear);
        program.setUniform("spotlights["+std::to_string(i)+"].quadratic", light->quadratic);
        program.setUniform("spotlights["+std::to_string(i)+"].ambient", light->ambient);
        program.setUniform("spotlights["+std::to_string(i)+"].diffuse", light->diffusion);
        program.setUniform("spotlights["+std::to_string(i)+"].specular", light->specular);
        i++;
    }
}