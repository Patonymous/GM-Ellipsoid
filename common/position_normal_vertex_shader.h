#ifndef POSITION_NORMAL_VERTEX_H
#define POSITION_NORMAL_VERTEX_H

struct VertexPositionNormal {
    float position[3];
    float normal[3];
};

struct VertexTriangleIndices {
    uint indices[3];
};

#endif // POSITION_NORMAL_VERTEX_H
