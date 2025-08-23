import lib.application;
import lib.utils;

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/ext.hpp>
#include <glm/glm.hpp>
#include <print>

class Application : public lib::Application {
public:
    Application()
        : lib::Application(640, 480, "Triangle!")
    {
        program = lib::createShaderProgram("vert.glsl", "frag.glsl");

        static float positions[] = {
            -1.0f, 1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, -1.0f, 1.0f, 1.0f, -1.0f, -1.0f, 1.0f, -1.0f,
            1.0f, -1.0f, -1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 1.0f, -1.0f,
            1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, -1.0f,
            1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
            -1.0f, -1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
            -1.0f, -1.0f, 1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f, 1.0f,
            -1.0f, -1.0f, -1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, 1.0f,
            -1.0f, -1.0f, 1.0f, 1.0f, -1.0f, 1.0f, 1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,
            -1.0f, 1.0f, -1.0f, 1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 1.0f,
            1.0f, 1.0f, 1.0f, -1.0f, 1.0f, 1.0f, -1.0f, 1.0f, -1.0f
        };
        glGenVertexArrays(1, vao);
        glBindVertexArray(vao[0]);
        glGenBuffers(2, vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions, GL_STATIC_DRAW);

        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
    }

    ~Application()
    {
        glDeleteVertexArrays(1, vao);
        glDeleteProgram(program);
    }

    virtual void render(double time) override
    {
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(program);

        // get the uniform variables for the MV and projection matrices
        mvLoc = glGetUniformLocation(program, "mv_matrix");
        pLoc = glGetUniformLocation(program, "p_matrix");

        // build perspective matrix
        aspect = float(width()) / float(height());
        pMat = glm::perspective(glm::radians(60.0f), aspect, 0.1f, 1000.0f);

        // build view matrix, model matrix, and model-view matrix
        vMat = glm::transpose(glm::mat4_cast(glm::quat(camRot))) * glm::translate(glm::mat4(1.0f), -camPos);
        mMat = glm::translate(glm::mat4(1.0f), cubePos);

        // copy perspective and MV matrices to corresponding uniform variables
        glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvMat));
        glUniformMatrix4fv(pLoc, 1, GL_FALSE, glm::value_ptr(pMat));
        mvMat = vMat * mMat;

        // copy perspective and MV matrices to corresponding uniform variables
        glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvMat));
        glUniformMatrix4fv(pLoc, 1, GL_FALSE, glm::value_ptr(pMat));

        // associate VBO with the corresponding vertex attribute in the vertex shader
        glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(0);

        // adjust OpenGL settings and draw model
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }

    void onKey(int key, int scancode, int action, int mods) override
    {
        float const moveStep = 0.1f;
        float const rotStep = glm::radians(1.0f);
        switch (key) {
        // Move
        case GLFW_KEY_W:
            camPos.z -= moveStep;
            break;
        case GLFW_KEY_S:
            camPos.z += moveStep;
            break;
        case GLFW_KEY_A:
            camPos.x -= moveStep;
            break;
        case GLFW_KEY_D:
            camPos.x += moveStep;
            break;
        case GLFW_KEY_Q:
            camPos.y -= moveStep;
            break;
        case GLFW_KEY_E:
            camPos.y += moveStep;
            break;

        // Rotate
        case GLFW_KEY_UP:
            camRot.x += rotStep;
            break; // pitch up
        case GLFW_KEY_DOWN:
            camRot.x -= rotStep;
            break; // pitch down
        case GLFW_KEY_LEFT:
            camRot.y += rotStep;
            break; // yaw left
        case GLFW_KEY_RIGHT:
            camRot.y -= rotStep;
            break; // yaw right
        case GLFW_KEY_Z:
            camRot.z += rotStep;
            break; // roll left
        case GLFW_KEY_X:
            camRot.z -= rotStep;
            break; // roll right
        }
    }

    void onCursorPos(double x, double y) override
    {
        cursorPos.x = x;
        cursorPos.y = y;
    }

private:
    glm::vec3 camPos { 0.0f, 0.0f, 8.0f };
    glm::vec3 camRot { 0.0f, 0.0f, 0.0f };
    glm::vec3 cubePos { 0.0f, -2.0f, 0.0f };
    GLuint program;
    GLuint vao[1];
    GLuint vbo[2];

    GLuint mvLoc, pLoc;
    float aspect;
    glm::mat4 pMat, vMat, mMat, mvMat;

    glm::vec2 cursorPos;
};

int main()
{
    Application app;
    app.run();
    return EXIT_SUCCESS;
}
