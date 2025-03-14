#version 330 core

uniform vec2 radius;
uniform mat4 pvm;

layout (location = 0) in vec2 params;

void main()
{
    float t = params.x, s = params.y;
    vec3 local = vec3(cos(t), 0.f, sin(t)) * radius.x
               + vec3(cos(t)*cos(s), sin(s), sin(t) * cos(s)) * radius.y;

    gl_Position = pvm * vec4(local, 1.0f);
}
