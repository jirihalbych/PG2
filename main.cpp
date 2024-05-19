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
using namespace glm;

int main() {
    glewExperimental = true; // Needed for core profile
    if( !glfwInit() )
    {
        fprintf( stderr, "Failed to initialize GLFW\n" );
        return -1;
    }
    glfwWindowHint(GLFW_SAMPLES, 4); // 4x antialiasing
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); //
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window;
    int width = 1024;
    int height = 768;// (In the accompanying source code, this variable is global for simplicity)
    window = glfwCreateWindow( width, height, "Tutorial 01", NULL, NULL);
    if( window == NULL ){
        fprintf( stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n" );
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
// Initialize GLEW
    glewExperimental=true; // Needed in core profile
    if (!glewInit()) {
        throw std::runtime_error("Cannot initialize glew");
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
    GLuint programID = ShaderProgram( "basic.vert", "basic.frag" ).ID;

    // Get a handle for our "MVP" uniform

    GLuint MatrixID = glGetUniformLocation(programID, "MVP");
    GLuint ViewMatrixID = glGetUniformLocation(programID, "V");
    GLuint ModelMatrixID = glGetUniformLocation(programID, "M");
    //GLuint ModelView3x3MatrixID = glGetUniformLocation(programID, "MV3x3");

    // Load the texture
    GLuint Texture = loadPNG("monu2.png");
    GLuint TextureID  = glGetUniformLocation(programID, "myTextureSampler");

    /*
    GLuint DiffuseTexture = loadDDS("diffuse.DDS");
    GLuint NormalTexture = loadBMP_custom("normal.bmp");
    GLuint SpecularTexture = loadDDS("specular.DDS");
    GLuint DiffuseTextureID  = glGetUniformLocation(programID, "DiffuseTextureSampler");
    GLuint NormalTextureID  = glGetUniformLocation(programID, "NormalTextureSampler");
    GLuint SpecularTextureID  = glGetUniformLocation(programID, "SpecularTextureSampler");
*/
    // Get a handle for our "myTextureSampler" uniform
    Model model = Model("monu2.obj");
    //Model model2 = Model("cube.obj");

    glUseProgram(programID);
    GLuint LightID = glGetUniformLocation(programID, "LightPosition_worldspace");
    GLuint AlphaID = glGetUniformLocation(programID, "Alpha");
    double lastTime = glfwGetTime();
    int nbFrames = 0;
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    do{
        float currentTime = glfwGetTime();
        float deltaTime = currentTime - lastTime;
        lastTime = currentTime;
        std::cout << "FPS: " << (1 / deltaTime) << "/" << deltaTime << std::endl;

        // Measure speed
        nbFrames++;
        if ( currentTime - lastTime >= 1.0 ){ // If last prinf() was more than 1 sec ago
            // printf and reset timer
            //printf("%f ms/frame\n", 1000.0/double(nbFrames));
            nbFrames = 0;
            lastTime += 1.0;
        }
        // Clear the screen. It's not mentioned before Tutorial 02, but it can cause flickering, so it's there nonetheless.
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
        glUseProgram(programID);
        computeMatricesFromInputs(window, width, height);
        glm::mat4 ProjectionMatrix = getProjectionMatrix();
        glm::mat4 ViewMatrix = getViewMatrix();
        glm::mat4 ModelMatrix = glm::mat4(1.0);
        //glm::mat4 ModelViewMatrix = ViewMatrix * ModelMatrix;
        //glm::mat3 ModelView3x3Matrix = glm::mat3(ModelViewMatrix);
        glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;


        // Send our transformation to the currently bound shader,
        // in the "MVP" uniform
        glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
        glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
        glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &ViewMatrix[0][0]);
       // glUniformMatrix3fv(ModelView3x3MatrixID, 1, GL_FALSE, &ModelView3x3Matrix[0][0]);

        glm::vec3 lightPos = glm::vec3(6,7,6);
        glUniform3f(LightID, lightPos.x, lightPos.y, lightPos.z);

        // Bind our texture in Texture Unit 0
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, Texture);
        glUniform1f(AlphaID, 1.0);
/*
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, DiffuseTexture);
        // Set our "DiffuseTextureSampler" sampler to use Texture Unit 0
        glUniform1i(DiffuseTextureID, 0);

        // Bind our normal texture in Texture Unit 1
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, NormalTexture);
        // Set our "NormalTextureSampler" sampler to use Texture Unit 1
        glUniform1i(NormalTextureID, 1);

        // Bind our specular texture in Texture Unit 2
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, SpecularTexture);
        // Set our "SpecularTextureSampler" sampler to use Texture Unit 2
        glUniform1i(SpecularTextureID, 2);
*/
        // Set our "myTextureSampler" sampler to use Texture Unit 0
        glUniform1i(TextureID, 0);
        model.draw();

        glm::mat4 ModelMatrix2 = glm::mat4(4.0);
        ModelMatrix2 = glm::translate(ModelMatrix2, glm::vec3(20.0f, 0.0f, 0.0f));
        glm::mat4 MVP2 = ProjectionMatrix * ViewMatrix * ModelMatrix2;

        // Send our transformation to the currently bound shader,
        // in the "MVP" uniform
        glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP2[0][0]);
        glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix2[0][0]);
        glUniform1f(AlphaID, 1.0);
       // model2.draw();
        // Swap buffers
        glfwSwapBuffers(window);
        glfwPollEvents();

    } // Check if the ESC key was pressed or the window was closed
    while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
           glfwWindowShouldClose(window) == 0 );
    model.clear();
    glDeleteProgram(programID);
    //glDeleteTextures(1, &Texture);
    glDeleteVertexArrays(1, &VertexArrayID);
    glfwTerminate();

    return 0;
}
