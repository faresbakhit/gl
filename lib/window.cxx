module;

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <print>
#include <stdexcept>
#include <string>
#include <utility>

export module lib.window;

namespace lib {

char const* platformName(int platform)
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

export class Window {
public:
    Window(int width, int height, std::string const& title)
        : width(width)
        , height(height)
    {
        glfwSetErrorCallback(glfwErrorCallback);

        if (!glfwInit()) {
            throw std::runtime_error("Failed to initialize GLFW");
        }

        std::println("[INFO] GLFW platform: {}", platformName(glfwGetPlatform()));

        {
            int major, minor, revision;
            glfwGetVersion(&major, &minor, &revision);
            std::println("[INFO] GLFW version: {}.{}.{}", major, minor, revision);
        }

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

        glfwWindow = glfwCreateWindow(width, height, title.c_str(), NULL, NULL);
        if (glfwWindow == NULL) {
            glfwTerminate();
            throw std::runtime_error("Failed to create GLFW window");
        }
        glfwSetWindowUserPointer(glfwWindow, this);
        glfwMakeContextCurrent(glfwWindow);

        {
            GLenum err = glewInit();
            if (err != GLEW_OK) {
                std::println(stderr, "[ERROR] GLEW: {}", (char const*)glewGetErrorString(err));
                glfwTerminate();
                throw std::runtime_error("Failed to initialize GLEW");
            }
            std::println("[INFO] GLEW version: {}", (char const*)glewGetString(GLEW_VERSION));
        }

        std::println("[INFO] OpenGL version: {}", (char const*)glGetString(GL_VERSION));
        std::println("[INFO] OpenGL renderer: {}", (char const*)glGetString(GL_RENDERER));
        std::println("[INFO] OpenGL vendor: {}", (char const*)glGetString(GL_VENDOR));

        glfwGetFramebufferSize(glfwWindow, &width, &height);
        glViewport(0, 0, width, height);

        glfwSetKeyCallback(glfwWindow, glfwKeyCallback);
        glfwSetCursorPosCallback(glfwWindow, glfwCursorPosCallback);
        glfwSetFramebufferSizeCallback(glfwWindow, glfwFramebufferSizeCallback);

        glfwSwapInterval(1);

        // TODO Move out of here

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);

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
        shaderProgram = glCreateProgram();
        glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
        glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
        glCompileShader(vertexShader);
        glCompileShader(fragmentShader);
        glAttachShader(shaderProgram, vertexShader);
        glAttachShader(shaderProgram, fragmentShader);
        glLinkProgram(shaderProgram);
    }

    ~Window()
    {
        glfwTerminate();
    }

    void run()
    {
        while (!glfwWindowShouldClose(glfwWindow)) {
            glClear(GL_COLOR_BUFFER_BIT);
            glUseProgram(shaderProgram);
            glBindVertexArray(VAO);
            glDrawArrays(GL_TRIANGLES, 0, 3);

            glfwSwapBuffers(glfwWindow);
            glfwPollEvents();
        }
    }

private:
    GLFWwindow* glfwWindow;
    int width, height;
    GLuint VBO, VAO, shaderProgram;
    float vertices[18] = {
        0.0f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f,
        -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f,
        0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f
    };

    void keyCallback(int key, int scancode, int action, int mods)
    {
        if ((key == GLFW_KEY_ESCAPE || key == GLFW_KEY_Q) && action == GLFW_PRESS)
            glfwSetWindowShouldClose(glfwWindow, GLFW_TRUE);
    }

    void framebufferSizeCallback(int newWidth, int newHeight)
    {
        width = newWidth;
        height = newHeight;
        glViewport(0, 0, width, height);
    }

    void cursorPosCallback(double xpos, double ypos)
    {
        xpos = 2.0 * xpos / width - 1.0;
        ypos = 1.0 - 2.0 * ypos / height;
        vertices[0] = xpos;
        vertices[1] = ypos + 0.5;
        vertices[6] = xpos - 0.25;
        vertices[7] = ypos - 0.5;
        vertices[12] = xpos + 0.25;
        vertices[13] = ypos - 0.5;
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
    }

    static void glfwErrorCallback(int error, char const* description)
    {
        std::println(stderr, "[ERROR] GLFW: {}", description);
    }

    static void glfwKeyCallback(GLFWwindow* glfwWindow, int key, int scancode, int action, int mods)
    {
        Window* window = static_cast<Window*>(glfwGetWindowUserPointer(glfwWindow));
        window->keyCallback(key, scancode, action, mods);
    }

    static void glfwFramebufferSizeCallback(GLFWwindow* glfwWindow, int width, int height)
    {
        Window* window = static_cast<Window*>(glfwGetWindowUserPointer(glfwWindow));
        window->framebufferSizeCallback(width, height);
    }

    static void glfwCursorPosCallback(GLFWwindow* glfwWindow, double xpos, double ypos)
    {
        Window* window = static_cast<Window*>(glfwGetWindowUserPointer(glfwWindow));
        window->cursorPosCallback(xpos, ypos);
    }
};

}
