#include <glad/glad.h> 
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "Shader.h"
#include "Mesh.h"
#include "Program.h"
#include "Texture.h"

#include <iostream>
#include <vector>

// Compare two images of the same resolution using a horizontal slider
// be careful to remove metadata rotations as those don't seem to be handled by the image loader stb_image

std::string cwd = "C:\\Users\\gruj\\source\\repos\\ImageCompare\\ImageCompare\\";
float upscaling_factor = 0.5;

void glfw_error_callback(int error, const char* description);

class ImageApp {
public:
    ImageApp(std::string path_1, std::string path_2);
    void render_loop();
private:
    GLFWwindow* window;
    uint32_t size_x = 800;
    uint32_t size_y = 600;

    std::vector<Vertex> vertices = {
        {glm::vec3(0.0,0.0,0.0), glm::vec3(0.0,1.0,0.0), glm::vec2(0.0,1.0)},
        {glm::vec3(0.0,0.0,1.0), glm::vec3(0.0,1.0,0.0), glm::vec2(0.0,0.0)},
        {glm::vec3(1.0,0.0,0.0), glm::vec3(0.0,1.0,0.0), glm::vec2(1.0,1.0)},
        {glm::vec3(1.0,0.0,1.0), glm::vec3(0.0,1.0,0.0), glm::vec2(1.0,0.0)}
    };
    std::vector<unsigned int> indices = { 0,1,2,1,3,2 };
    Mesh view_plane;

    std::shared_ptr<Program> program;

    TextureP img1;
    TextureP img2;

    void set_resolution(std::string path_1, std::string path_2);
    void glfw_init();
    void glad_init();
    void gl_init(std::string path_1, std::string path_2);
};

ImageApp::ImageApp(std::string path_1, std::string path_2) {
    set_resolution(path_1, path_2);
    glfw_init();
    glad_init();
    gl_init(path_1, path_2);
}

void ImageApp::set_resolution(std::string path_1, std::string path_2) {
    int w1, h1;
    get_texture_size_from_path(path_1, w1, h1);
    int w2, h2;
    get_texture_size_from_path(path_2, w2, h2);
    
    assert(w1 == w2 && h1 == h2, "Image size mismatch");

    size_x = w1 * upscaling_factor;
    size_y = h1 * upscaling_factor;

    std::cout << size_x << "x" << size_y << std::endl;
}

void ImageApp::glfw_init()
{
    if (!glfwInit()) {
        std::cerr << "GLFW Initalization failed" << std::endl;
        exit(-1);
    }
    glfwSetErrorCallback(glfw_error_callback);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(size_x, size_y, "ImageCompare", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        exit(-1);
    }
    glfwMakeContextCurrent(window);
}

void ImageApp::glad_init()
{
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        exit(-1);
    }
}

void ImageApp::gl_init(std::string path_1, std::string path_2)
{
    glViewport(0, 0, size_x, size_y);
    view_plane = Mesh(vertices, indices);

    std::vector<std::shared_ptr<Shader>> post_shaders;
    post_shaders.push_back(std::make_shared<Shader>(GL_VERTEX_SHADER, (cwd+"shaders/shader.vs").c_str()));
    post_shaders.push_back(std::make_shared<Shader>(GL_FRAGMENT_SHADER, (cwd+"shaders/shader.fs").c_str()));
    program = std::make_shared<Program>(post_shaders);
    
    img1 = texture_from_path(path_1, "img1");
    img2 = texture_from_path(path_2, "img2");

    program->use();
    img1->bind_texture_unit(*program, 0);
    img2->bind_texture_unit(*program, 1);
}

void ImageApp::render_loop()
{
    while (!glfwWindowShouldClose(window))
    {
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);

        float x_frac = fmin(fmax(xpos, 0), size_x) / size_x;

        glClearColor(0, 0.4, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        program->use();
        program->set1f("mouse_x", x_frac);
        view_plane.draw();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();
}

int main(int argc, char* argv[]) {
    std::string path_1 = cwd + "test1.png";
    std::string path_2 = cwd + "test2.png";

    if (argc == 1) {
        std::cout << "No images provided" << std::endl;
    }
    else {
        assert(argc == 3, "Expected two image paths as arguments");
        path_1 = std::string(argv[1]);
        path_2 = std::string(argv[2]);
    }
    
    std::cout << "Input scaling factor:" << std::endl;
    std::cin >> upscaling_factor;

    ImageApp app{ path_1, path_2 };
    app.render_loop();
}

void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "GLFW Error: %s\n", description);
}