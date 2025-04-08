#version 330 core

in vec3 vsPosition0;
in vec3 vsPosition1;
in vec3 vsPosition2;
in vec3 vsPosition3;
in int vsRank; // in range 1-3

out Curve {
    vec3 position0;
    vec3 position1;
    vec3 position2;
    vec3 position3;
    int rank; // in range 1-3
} gs;

void main()
{
    gl_Position = vec4(0.f, 0.f, 0.f, 1.f);
    gs.position0 = vsPosition0;
    gs.position1 = vsPosition1;
    gs.position2 = vsPosition2;
    gs.position3 = vsPosition3;
    gs.rank = vsRank;
}
