#version 330 core

layout (location = 0) in int vsIndex;
layout (location = 1) in int vsRank; // in range 1-3

out Segment {
    int index;
    int rank; // in range 1-3
} gs;

void main()
{
    gl_Position = vec4(0.f, 0.f, 0.f, 1.f);
    gs.index = vsIndex;
    gs.rank = vsRank;
}
