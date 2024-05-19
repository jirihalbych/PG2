#include <cstddef>
#include <GLFW/glfw3.h>
#include "directional_light.h"
#include "ambient_light.h"
#include "spotlight.h"

class App {
public:
    GLFWwindow * window;
    ShaderProgram program;
    Model models [5];
    GLuint textures [5];
    static Model *chest;
    static Model *open;
    static Model *closed;
    static float volume;
    std::vector<DirectionalLight*> directionalLights;
    std::vector<AmbientLight*> ambientLights;
    std::vector<Spotlight*> spotlights;

    App();

    bool init(void);
    int run(void);
    static void error_callback(int, const char*);
    static void scroll_callback(GLFWwindow*, double, double);
    static void key_callback(GLFWwindow*, int, int, int, int);
    static void fbsize_callback(GLFWwindow *window, int width, int height);
    static void mouse_button_callback(GLFWwindow *window, int button, int action, int mods);
    static void cursor_position_callback(GLFWwindow *window, double xpos, double ypos);
    void addDirectional(DirectionalLight &light);
    void addAmbient(AmbientLight &light);
    void addSpotlight(Spotlight &light);
    void renderLights(ShaderProgram program);

    ~App();
private:

};