#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include "shaders.cpp"
#include "texture.cpp"
#include "controls.cpp"
#include "objloader.cpp"
#include "vboindexer.cpp"
#include "model.cpp"
#include "callbacks.cpp"
#include <glm/gtc/matrix_transform.hpp>
#include "gl_err_callback.cpp"
#include "include/ik_ISoundEngine.h"
#include "include/irrKlang.h"
#include "lights.cpp"

using namespace irrklang;

Model* App::chest;
Model* App::open;
Model* App::closed;
float App::volume = 0.5;

App::App() {
    // default constructor
    // nothing to do here (so far...)
    std::cout << "Constructed...\n";
}

bool App::init() {
    glewExperimental = true; // Needed for core profile
    if( !glfwInit() )
    {
        fprintf( stderr, "Failed to initialize GLFW\n" );
        return -1;
    }
    glfwWindowHint(GLFW_SAMPLES, 4); // 4x antialiasing
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    width = 1248;
    height = 768;
    window = glfwCreateWindow( 1248, 768, "Aplikace", NULL, NULL);
    if( window == NULL ){
        fprintf( stderr, "Failed to open GLFW window.\n" );
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, fbsize_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetErrorCallback(error_callback);

// Initialize GLEW
    glewExperimental=true; // Needed in core profile
    if (!glewInit()) {
        throw std::runtime_error("Cannot initialize glew");
    }
    if (GLEW_ARB_debug_output) {
        glDebugMessageCallback(MessageCallback, nullptr);
        glEnable(GL_DEBUG_OUTPUT);

        //default is asynchronous debug output, use this to simulate glGetError() functionality
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);

        std::cout << "GL_DEBUG enabled." << std::endl;
    } else {
        std::cout << "GL_DEBUG NOT SUPPORTED!" << std::endl;
    }
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    // Hide the mouse and enable unlimited movement
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Set the mouse at the center of the screen
    glfwPollEvents();
    glfwSetCursorPos(window, width/2, height/2);

    // Dark blue background
    glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

    // Enable depth test
    glEnable(GL_DEPTH_TEST);
    // Accept fragment if it is closer to the camera than the former one
    glDepthFunc(GL_LESS);

    // Cull triangles which normal is not towards the camera
    glDisable(GL_CULL_FACE);

    GLuint VertexArrayID;
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);

    // Create and compile our GLSL program from the shaders
    program = ShaderProgram( "basic.vert", "basic.frag" );

    // Load the texture
    textures[0] = loadPNG("./monu2.png");

    // Get a handle for our "myTextureSampler" uniform
    models[0] = Model("monu2.obj");
    models[1] = Model("Ghost.obj");
    textures[1] = loadPNG("./Ghost.png");
    models[2] = Model("Robot.vox-15.obj");
    textures[2] = loadPNG("./Robot.vox-15.png");
    models[3] = Model("closed.obj");
    textures[3] = loadPNG("./chest.png");
    models[4] = Model("open.obj");
    closed = &models[3];
    chest = &models[3];
    open = &models[4];

    program.activate();
   // LightID = glGetUniformLocation(program.ID, "LightPosition_worldspace");
    DirectionalLight* directionalLight;
    directionalLight = new DirectionalLight;
    directionalLight->direction = glm::vec3(0,-1,1);
    directionalLight->diffusion = glm::vec3(2.0f);
    addDirectional(*directionalLight);
    AmbientLight* ambientLight;
    ambientLight = new AmbientLight;
    ambientLight->color = glm::vec3(1,1,1);
    ambientLight->intensity = 0.05;
    addAmbient(*ambientLight);
    Spotlight* spotlight;
    spotlight = new Spotlight;
    spotlight->position = glm::vec3(0.0f, 12.0f, -4.0f);
    spotlight->direction = glm::vec3(0.0f, -1.0f, 0.0f);
    addSpotlight(*spotlight);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    return true;
}
double lastTime = glfwGetTime();
double alphaTime = glfwGetTime();
float alpha = 1.0;
float x = -4.0f;
float y = 5.0f;
bool glow = true;
bool fly = true;
float uhel = 0.0;
int App::run(void) {
    ISoundEngine* engine = createIrrKlangDevice();
    ISound* music = engine->play3D("./windchimes.mp3",
                                   vec3df(-2,2,2), true, false, true);
    music->setMinDistance(5.0f);

    while (!glfwWindowShouldClose(window)) {
        music->setVolume(volume);

        float currentTime = glfwGetTime();
    float deltaTime = currentTime - lastTime;
    lastTime = currentTime;
    std::cout << "FPS: " << (1 / deltaTime) << "/" << deltaTime << std::endl;
    // Clear the screen. It's not mentioned before Tutorial 02, but it can cause flickering, so it's there nonetheless.
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    program.activate();
    computeMatricesFromInputs(window);
    glm::mat4 ProjectionMatrix = getProjectionMatrix();
    glm::mat4 ViewMatrix = getViewMatrix();
    glm::mat4 ModelMatrix = glm::mat4(1.0);
    //glm::mat4 ModelViewMatrix = ViewMatrix * ModelMatrix;
    //glm::mat3 ModelView3x3Matrix = glm::mat3(ModelViewMatrix);
    glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;

    program.setUniform("MVP", MVP);
    program.setUniform("M", ModelMatrix);
    program.setUniform("V", ViewMatrix);

    program.setUniform("LightPosition_worldspace", vec3(cos(uhel)*3,7,sin(uhel)*3));
    uhel += 0.01;
    renderLights(program);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textures[0]);
    program.setUniform("Alpha",1.0f);

    program.setUniform("myTextureSampler", 0);
    models[0].draw();

    glm::mat4 ModelMatrix2 = glm::mat4(1.0);
    ModelMatrix2 = glm::rotate((glm::translate(glm::scale(ModelMatrix2,glm::vec3(0.5,0.5,0.5)), glm::vec3(x, 3.0f, 0.0f))),glm::radians(180.0f),glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 MVP2 = ProjectionMatrix * ViewMatrix * ModelMatrix2;
        program.setUniform("MVP", MVP2);
        program.setUniform("M", ModelMatrix2);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textures[1]);
        if (glfwGetTime() - alphaTime>0.1 && glow){
            alphaTime = glfwGetTime();
            alpha = alpha - 0.05;
            x = x - 0.01;
            if (alpha < 0.15){
                glow = false;
            }
        } else if (glfwGetTime() - alphaTime>0.1){
            alphaTime = glfwGetTime();
            alpha = alpha + 0.05;
            if (alpha >= 1.0){
                glow = true;
                alphaTime += 2;
            }
        }
        program.setUniform("Alpha",alpha);
        program.setUniform("myTextureSampler",0);
        glm::vec3 position = getPosition();
        glm::vec3 direction = getDirection();
        engine->setListenerPosition(vec3df(position.x,position.y,position.z),
                                    vec3df(direction.x,direction.y,direction.z));

        models[1].draw();
        glm::mat4 ModelMatrix3 = glm::mat4(1.0);
        ModelMatrix3 = glm::translate(glm::scale(ModelMatrix3,glm::vec3(0.5,0.5,0.5)), glm::vec3(-2.0f, y, 0.0f));
        glm::mat4 MVP3 = ProjectionMatrix * ViewMatrix * ModelMatrix3;
        program.setUniform("MVP", MVP3);
        program.setUniform("M", ModelMatrix3);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textures[2]);
        program.setUniform("Alpha",1.0f);
        program.setUniform("myTextureSampler",0);
        models[2].draw();
        if (y < 6.0 && fly){
            y += 0.001;
        } else if (y < 4) {
            fly = true;
        } else if (y > 6.0){
            y -= 0.001;
            fly = false;
        } else {
            y -= 0.001;
        }

        glm::mat4 ModelMatrix4 = glm::mat4(1.0);
        ModelMatrix4 = glm::translate(glm::scale(ModelMatrix4,glm::vec3(0.5,0.5,0.5)), glm::vec3(-2.0f, 3, -2.0f));
        glm::mat4 MVP4 = ProjectionMatrix * ViewMatrix * ModelMatrix4;
        program.setUniform("MVP", MVP4);
        program.setUniform("M", ModelMatrix4);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textures[3]);
        program.setUniform("Alpha",1.0f);
        program.setUniform("myTextureSampler",0);
        chest->draw();

        // Swap buffers
    glfwSwapBuffers(window);
    glfwPollEvents();
}
    if (window)
        glfwDestroyWindow(window);
    models[0].clear();
    glDeleteProgram(program.ID);
    glDeleteTextures(1, &textures[0]);
    glfwTerminate();
    program.clear();;
}

App::~App() {
    // clean-up
    //mesh.clear();
    std::cout << "Bye...\n";
    exit(EXIT_SUCCESS);
}

// define our application
App app;

int main() {
    if (app.init())
        app.run();
}
