#include "dotto/pch.h"
#include "dotto/program.hpp"
#include "dotto/input.hpp"
#include "dotto/ui/asset.hpp"
#include "dotto/audio/audio.hpp"
#include "dotto/scene/view.hpp"
#include "dotto/util/console.hpp"
#include "dotto/globals.hpp"

using namespace std::chrono_literals;

constexpr std::chrono::nanoseconds timestep(16ms);

// Prints GL errors
void gl_debug_callback(
    GLenum source,
    GLenum type,
    GLuint id,
    GLenum severity,
    GLsizei length,
    const GLchar *message,
    const void *userParam
) {
    if (severity == GL_DEBUG_SEVERITY_NOTIFICATION)
        return;

    std::cerr << "[OPENGL:(" << source << ", " << type << ", " << id << ", "
        << severity << ")]: " << message << "\n";
}

boolean fullscreen = false;

int main (int argc, char** argv) {
    // init io & enable file logging
    dotto::io::init();
    dotto::console::enable_file();

    // Initialise GLFW
    if(!glfwInit())
    {
        fprintf(stderr, "Failed to initialize GLFW\n");
        return -1;
    }

    // CONFIGURE THINGS
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // We want OpenGL 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // We don't want the old OpenGL

    // BUILD WINDOW
    // The monitor we will be displaying to.
    GLFWmonitor* monitor    = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);
    GLFWwindow* window;
    // Go fullscreen or windowed.
    if (fullscreen)
    {
        dotto::D_WIDTH = mode->width;
        dotto::D_HEIGHT = mode->height;
        window = glfwCreateWindow(
            dotto::D_WIDTH, dotto::D_HEIGHT, "dotto", monitor, NULL
        );
    }
    else
        window = glfwCreateWindow(dotto::D_WIDTH, dotto::D_HEIGHT, "dotto", NULL, NULL);

    glfwMakeContextCurrent(window); // Initialize GLEW

    // INIT GLEW
    glewExperimental = true; // Needed in core profile
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to initialize GLEW\n");
        return -1;
    }

    // BUILD RENDER DATA
    GLuint VertexArrayID;
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);

    // Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
    glm::mat4 projection_matrix = glm::perspective(
        glm::radians(90.0f), (float) 1280 / (float) 720, 0.1f, 100.0f
    );

    // Camera matrix
    glm::mat4 view_matrix = glm::lookAt(
        glm::vec3(0.0f, 0.0f, 1.0f),
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 1.0f, 0.0f)
    );

    // INIT COMPONENTS
    dotto::view::init();
    dotto::ui::asset::init();
    dotto::audio::init();
    dotto::input::init(window);

    // INIT SCENES
    dotto::view::load();

    // RENDER LOOP
    float delta_time = 0.0f;
    while (glfwWindowShouldClose(window) == 0 && !dotto::should_exit)
    {
        auto start = std::chrono::high_resolution_clock::now();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // render meshes
        for (auto mesh : *dotto::view::live_meshes)
            mesh->render(projection_matrix, view_matrix);

        glfwSwapBuffers(window);
        glfwPollEvents();
        auto end = std::chrono::high_resolution_clock::now();
        auto length_ns = end - start;
        auto wait_ns = timestep - length_ns;
        std::this_thread::sleep_for(wait_ns);
        delta_time = (wait_ns + length_ns).count() / 1000000000.0;
    }

    dotto::view::clean();
    dotto::audio::clean();
    dotto::console::clean();

    return EXIT_SUCCESS;
}
