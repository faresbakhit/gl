#version 430 core

out vec4 color;
layout (location = 0) in vec3 position;

void main(void)
{
    if (abs(position.x) == 1.0){
        color = vec4(1.0, 0.0, 0.0, 1.0);
    } else if (abs(position.y) == 1.0){
        color = vec4(0.0, 0.0, 1.0, 1.0);
    } else {
        color = vec4(0.0, 1.0, 0.0, 1.0);
    }
}
