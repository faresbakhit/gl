module;

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <cmath>
#include <glm/glm.hpp>
#include <print>
#include <stdexcept>
#include <string>
#include <utility>

export module lib.application;

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

namespace lib {

export class Application {
public:
    GLFWwindow* window;

    Application(int width, int height, std::string const& title)
        : mWidth(width)
        , mHeight(height)
    {
        glfwSetErrorCallback(glfwErrorCallback);

        if (!glfwInit()) {
            throw std::runtime_error("Failed to initialize GLFW");
        }

        {
            int major, minor, revision;
            glfwGetVersion(&major, &minor, &revision);
            std::println("[INFO] GLFW version: {}.{}.{}", major, minor, revision);
        }

        std::println("[INFO] GLFW platform: {}", platformName(glfwGetPlatform()));

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

        window = glfwCreateWindow(width, height, title.c_str(), NULL, NULL);
        if (window == NULL) {
            glfwTerminate();
            throw std::runtime_error("Failed to create GLFW window");
        }
        glfwSetWindowUserPointer(window, this);
        glfwMakeContextCurrent(window);

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

        glfwGetFramebufferSize(window, &width, &height);
        glViewport(0, 0, width, height);

        glfwSetKeyCallback(window, glfwKeyCallback);
        glfwSetCursorPosCallback(window, glfwCursorPosCallback);
        glfwSetFramebufferSizeCallback(window, glfwFramebufferSizeCallback);

        setVsync(true);
    }

    virtual ~Application()
    {
        glfwTerminate();
    }

    void run()
    {
        while (!glfwWindowShouldClose(window)) {
            render(glfwGetTime());
            glfwSwapBuffers(window);
            glfwPollEvents();
        }
    }

    virtual void render(double time)
    {
    }

    virtual void onKey(int key, int scancode, int action, int mods)
    {
        if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
            glfwSetWindowShouldClose(window, GLFW_TRUE);
    }

    virtual void onFramebufferSize(int newWidth, int newHeight)
    {
        mWidth = newWidth;
        mHeight = newHeight;
        glViewport(0, 0, mWidth, mHeight);
    }

    virtual void onCursorPos(double xpos, double ypos) { }

    int width() const { return mWidth; }
    int height() const { return mHeight; }

    void setVsync(bool enable)
    {
        mVsync = enable;
        glfwSwapInterval((int)mVsync);
    }

private:
    int mWidth;
    int mHeight;
    bool mVsync;

    static void glfwErrorCallback(int error, char const* description)
    {
        std::println(stderr, "[ERROR] GLFW[{}]: {}", error, description);
    }

    static void glfwKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
    {
        Application* app = static_cast<Application*>(glfwGetWindowUserPointer(window));
        app->onKey(key, scancode, action, mods);
    }

    static void glfwFramebufferSizeCallback(GLFWwindow* window, int width, int height)
    {
        Application* app = static_cast<Application*>(glfwGetWindowUserPointer(window));
        app->onFramebufferSize(width, height);
    }

    static void glfwCursorPosCallback(GLFWwindow* window, double xpos, double ypos)
    {
        Application* app = static_cast<Application*>(glfwGetWindowUserPointer(window));
        app->onCursorPos(xpos, ypos);
    }
};

}
