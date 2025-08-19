#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <print>
#include <utility>

static void error_callback(int error, char const* description)
{
    std::println(stderr, "[ERROR] GLFW: {}", description);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if ((key == GLFW_KEY_ESCAPE || key == GLFW_KEY_Q) && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}

static void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

char const* platform_name(int platform)
{
    switch (platform) {
    case GLFW_PLATFORM_WIN32:
        return "Win32";
    case GLFW_PLATFORM_COCOA:
        return "Cocoa";
    case GLFW_PLATFORM_WAYLAND:
        return "Wayland";
    case GLFW_PLATFORM_X11:
        return "X11";
    case GLFW_PLATFORM_NULL:
        return "None";
    default:
        std::unreachable();
    }
}

int main(void)
{
    glfwSetErrorCallback(error_callback);

    {
        int major, minor, revision;
        glfwGetVersion(&major, &minor, &revision);
        std::println("[INFO] GLFW version: {}.{}.{}", major, minor, revision);
    }

    if (!glfwInit()) {
        return EXIT_FAILURE;
    }

    std::println("[INFO] GLFW platform: {}", platform_name(glfwGetPlatform()));

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow* window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (window == NULL) {
        glfwTerminate();
        return EXIT_FAILURE;
    }
    glfwMakeContextCurrent(window);

    {
        GLenum err = glewInit();
        if (err != GLEW_OK) {
            std::println(stderr, "[ERROR] GLEW: {}", (char const*)glewGetErrorString(err));
            glfwTerminate();
            return EXIT_FAILURE;
        }
        std::println("[INFO] GLEW version: {}", (char const*)glewGetString(GLEW_VERSION));
    }

    glfwSwapInterval(1);

    glfwSetKeyCallback(window, key_callback);

    {
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        glViewport(0, 0, width, height);
        glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    }

    float vertices[] = {
        0.0f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f,
        -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f,
        0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f
    };

    GLuint VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    char const* vertexShaderSource = "#version 330 core\n"
                                     "layout (location = 0) in vec3 aPos;"
                                     "layout (location = 1) in vec3 aColor;"
                                     "out vec3 vColor;"
                                     "void main()"
                                     "{"
                                     "    gl_Position = vec4(aPos, 1.0);"
                                     "    vColor = aColor;"
                                     "}";

    char const* fragmentShaderSource = "#version 330 core\n"
                                       "in vec3 vColor;"
                                       "out vec4 FragColor;"
                                       "void main()"
                                       "{"
                                       "    FragColor = vec4(vColor, 1.0);"
                                       "}";

    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    GLuint shaderProgram = glCreateProgram();
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(vertexShader);
    glCompileShader(fragmentShader);
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT);
        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    glfwTerminate();
    return EXIT_SUCCESS;
}
