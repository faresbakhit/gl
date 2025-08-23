#version 430 core

uniform mat4 mv_matrix;
uniform mat4 p_matrix;
layout (location = 0) in vec3 position;
layout (location = 0) out vec3 out_position;

void main(void)
{
    gl_Position = p_matrix * mv_matrix * vec4(position, 1.0);
    out_position = position;
}
