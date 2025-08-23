module;

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <cerrno>
#include <fstream>
#include <initializer_list>
#include <print>
#include <system_error>
#include <vector>

export module lib.utils;

namespace lib {

std::vector<char> getShaderInfoLog(GLuint shader)
{
    GLint infoLogLength;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);
    if (infoLogLength == 0) {
        char const NO_LOG[] = "No information log";
        return { NO_LOG, NO_LOG + sizeof(NO_LOG) };
    }
    std::vector<GLchar> infoLog(infoLogLength);
    glGetShaderInfoLog(shader, infoLogLength, NULL, infoLog.data());
    return infoLog;
}

std::vector<char> getProgramInfoLog(GLuint program)
{
    GLint infoLogLength;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLength);
    if (infoLogLength == 0) {
        char const NO_LOG[] = "No information log";
        return { NO_LOG, NO_LOG + sizeof(NO_LOG) };
    }
    std::vector<GLchar> infoLog(infoLogLength);
    glGetProgramInfoLog(program, infoLogLength, NULL, infoLog.data());
    return infoLog;
}

GLuint compileShader(std::string const& shaderPath, GLenum type)
{
    GLint shaderCompiled;
    std::ifstream shaderFile(shaderPath);
    if (!shaderFile) {
        std::println("[ERROR] IO: {}: {}", shaderPath,
            std::error_code(errno, std::generic_category()).message());
        return -1;
    }
    std::vector vertexShaderSource(
        (std::istreambuf_iterator<GLchar>(shaderFile)),
        std::istreambuf_iterator<GLchar>());
    GLchar const* shaderSourceData = vertexShaderSource.data();
    GLint const shaderSourceLength = vertexShaderSource.size();
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &shaderSourceData, &shaderSourceLength);
    glCompileShader(shader);
    glGetShaderiv(shader, GL_COMPILE_STATUS, &shaderCompiled);
    if (shaderCompiled != GL_TRUE) {
        glDeleteShader(shader);
        return -1;
    }
    return shader;
}

GLuint linkShaders(std::initializer_list<GLuint> shaders)
{
    GLint programLinked;
    GLuint program = glCreateProgram();
    for (auto shader : shaders) {
        glAttachShader(program, shader);
    }
    glLinkProgram(program);
    for (auto shader : shaders) {
        glDeleteShader(shader);
    }
    glGetProgramiv(program, GL_LINK_STATUS, &programLinked);
    if (programLinked != GL_TRUE) {
        std::println("[ERROR] GL: Linker: {}", getProgramInfoLog(program).data());
        glDeleteProgram(program);
        return -1;
    }
    return program;
}

export GLuint createShaderProgram(
    std::string const& vertexShaderPath,
    std::string const& fragmentShaderPath)
{
    return linkShaders({
        compileShader(vertexShaderPath, GL_VERTEX_SHADER),
        compileShader(fragmentShaderPath, GL_FRAGMENT_SHADER),
    });
}

export GLuint createShaderProgram(
    std::string const& vertexShaderPath,
    std::string const& geometryShaderPath,
    std::string const& fragmentShaderPath)
{
    return linkShaders({
        compileShader(vertexShaderPath, GL_VERTEX_SHADER),
        compileShader(geometryShaderPath, GL_GEOMETRY_SHADER),
        compileShader(fragmentShaderPath, GL_FRAGMENT_SHADER),
    });
}

export GLuint createShaderProgram(
    std::string const& vertexShaderPath,
    std::string const& tessControlShaderPath,
    std::string const& tessEvaluationShaderPath,
    std::string const& fragmentShaderPath)
{
    return linkShaders({
        compileShader(vertexShaderPath, GL_VERTEX_SHADER),
        compileShader(tessControlShaderPath, GL_TESS_CONTROL_SHADER),
        compileShader(tessEvaluationShaderPath, GL_TESS_EVALUATION_SHADER),
        compileShader(fragmentShaderPath, GL_FRAGMENT_SHADER),
    });
}

export GLuint createShaderProgram(
    std::string const& vertexShaderPath,
    std::string const& tessControlShaderPath,
    std::string const& tessEvaluationShaderPath,
    std::string const& geometryShaderPath,
    std::string const& fragmentShaderPath)
{
    return linkShaders({
        compileShader(vertexShaderPath, GL_VERTEX_SHADER),
        compileShader(tessControlShaderPath, GL_TESS_CONTROL_SHADER),
        compileShader(tessEvaluationShaderPath, GL_TESS_EVALUATION_SHADER),
        compileShader(geometryShaderPath, GL_GEOMETRY_SHADER),
        compileShader(fragmentShaderPath, GL_FRAGMENT_SHADER),
    });
}

}
