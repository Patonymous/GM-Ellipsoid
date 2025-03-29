#version 330 core

uniform mat4 pv;
uniform mat4 model;
uniform mat4 ti_model;

layout (location = 0) in vec3 vsPosition;
layout (location = 1) in vec3 vsNormal;

out vec3 fsPosition;
out vec3 fsNormal;

void main()
{
    vec4 worldPosition = model * vec4(vsPosition, 1.0f);
    gl_Position = pv * worldPosition;

    fsPosition = worldPosition.xyz;
    fsNormal = normalize((ti_model * vec4(vsNormal, 0.f)).xyz);
}
