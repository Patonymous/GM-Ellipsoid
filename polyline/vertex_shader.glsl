#version 330 core

uniform mat4 pv;

in vec3 position;

void main()
{
    gl_Position = pv * vec4(position, 1.0f);
}
