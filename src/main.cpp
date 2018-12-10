#include "dotto/pch.h"
#include "dotto/program.hpp"
#include "dotto/ui/rect.hpp"
#include "dotto/ui/texture.hpp"

#define DR_MP3_IMPLEMENTATION
#include "drsoft/dr_mp3.h"   // Enables MP3 decoding.
#define MINI_AL_IMPLEMENTATION
#include "drsoft/mini_al.h"

#include <stdio.h>

using namespace std::chrono_literals;

constexpr std::chrono::nanoseconds timestep(16ms);

// This is the function that's used for sending more data to the device for playback.
mal_uint32 on_send_frames_to_device(mal_device* pDevice, mal_uint32 frameCount, void* pSamples)
{
    mal_decoder* pDecoder = (mal_decoder*)pDevice->pUserData;
    if (pDecoder == NULL) {
        return 0;
    }

    return (mal_uint32)mal_decoder_read(pDecoder, frameCount, pSamples);
}

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

int main(int argc, char** argv) {
    // Initialise GLFW
    glewExperimental = true; // Needed for core profile
    if( !glfwInit() )
    {
        fprintf( stderr, "Failed to initialize GLFW\n" );
        return -1;
    }

    // CONFIGURE THINGS
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // We want OpenGL 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // We don't want the old OpenGL

    // BUILD WINDOW
    // Open a window and create its OpenGL context
    GLFWwindow* window; // (In the accompanying source code, this variable is global for simplicity)
    window = glfwCreateWindow(1280, 720, "dotto", NULL, NULL);
    if( window == NULL ){
        fprintf( stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n" );
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window); // Initialize GLEW
    glewExperimental = true; // Needed in core profile
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to initialize GLEW\n");
        return -1;
    }

    // Ensure we can capture the escape key being pressed below
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

    // BUILD RENDER DATA
    GLuint VertexArrayID;
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);

    // scene objects
    std::vector<dotto::ui::rect*> meshes;

    // create rect
    GLuint prog_id = dotto::pipeline::create_program(
        "res\\shaders\\default.vert",
        "res\\shaders\\default.frag"
    );

    dotto::ui::texture tex("res\\graphics\\konata.png");
    dotto::ui::rect testo(prog_id, &tex);
    // add rect
    meshes.push_back(&testo);

    // Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
    glm::mat4 projection_matrix = glm::perspective(
        glm::radians(90.0f),
        (float) 1280 / (float) 720,
        0.1f,
        100.0f
    );

    // Camera matrix
    glm::mat4 view_matrix = glm::lookAt(
        glm::vec3(0.0f, 0.0f, 1.0f),
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 1.0f, 0.0f)
    );

    mal_decoder decoder;
    mal_result result = mal_decoder_init_file(
        dotto::io::file::make_relative("res\\audio\\soraw.mp3").c_str(),
        NULL,
        &decoder
    );

    if (result != MAL_SUCCESS) {
        return -2;
    }

    mal_device_config config = mal_device_config_init_playback(
        decoder.outputFormat,
        decoder.outputChannels,
        decoder.outputSampleRate,
        on_send_frames_to_device
    );

    mal_device device;
    if (mal_device_init(NULL, mal_device_type_playback, NULL, &config, &decoder, &device) != MAL_SUCCESS) {
        std::cout << "Failed to open playback device.\n";
        mal_decoder_uninit(&decoder);
        return -3;
    }

    if (mal_device_start(&device) != MAL_SUCCESS) {
        std::cout << "Failed to start playback device.\n";
        mal_device_uninit(&device);
        mal_decoder_uninit(&decoder);
        return -4;
    }

    // RENDER LOOP
    float delta_time = 0.0f;
    while(
        glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
        glfwWindowShouldClose(window) == 0
    ) {
        auto start = std::chrono::high_resolution_clock::now();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // render meshes
        for(auto mesh : meshes)
            mesh->render(projection_matrix, view_matrix);

        glfwSwapBuffers(window);
        glfwPollEvents();
        auto end = std::chrono::high_resolution_clock::now();
        auto length_ns = end - start;
        auto wait_ns = timestep - length_ns;
        std::this_thread::sleep_for(wait_ns);
        delta_time = (wait_ns + length_ns).count() / 1000000000.0;
    }

    mal_device_uninit(&device);
    mal_decoder_uninit(&decoder);

    return EXIT_SUCCESS;
}
