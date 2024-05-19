#include <iostream>
#include "app.h"

void App::error_callback(int error, const char *description) {
    fprintf(stderr, "Error: %s\n", description);
}

void App::scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
    if (yoffset > 0.0) {
        if (App::volume < 1.0){
         App::volume += 0.05;}
    } else {
        if (App::volume > 0.0){
            App::volume -= 0.05;}

    }
}

int vsync = 1;
int xpos = 0;
int ypos = 0;
int fullscreen = 0;
int w = 1248;
int h = 768;
void App::key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_F && action == GLFW_PRESS && fullscreen == 0){
        glfwGetWindowPos(window, &xpos, &ypos);
        glfwGetWindowSize(window, &w, &h);
        const GLFWvidmode * mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
        glfwSetWindowMonitor(window, NULL, xpos, ypos, mode->width, mode->height, 0);
        fullscreen = 1;
    } else if  (key == GLFW_KEY_F && action == GLFW_PRESS){
        glfwSetWindowMonitor(window, NULL, xpos, ypos, w, h, 0);
        fullscreen = 0;
    }
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    if (key == GLFW_KEY_V && action == GLFW_PRESS) {
        if (vsync == 1) {
            vsync = 0;
            glfwSwapInterval(0);
        } else {
            vsync = 1;
            glfwSwapInterval(1);
        }
    }
}


void App::fbsize_callback(GLFWwindow* window, int width, int height){
    setHeight(height);
    setWidth(width);
    glViewport(xpos, ypos, width, height);
}

void App::mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
    {
        App::chest = App::open;
    }

    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
        App::chest = App::closed;
    }
}

void App::cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
}
