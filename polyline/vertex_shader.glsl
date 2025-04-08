#version 330 core

in vec3 vsPositions[4];
in int vsRank; // in range 1-3

out Curve {
    vec3 positions[4];
    int rank; // in range 1-3
} gs;

void main()
{
    gl_Position = vec4(0.f, 0.f, 0.f, 1.f);
    gs.positions = vsPositions;
    gs.rank = vsRank;
}
