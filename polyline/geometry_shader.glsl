#version 330 core

layout (points) in;
layout (line_strip, max_vertices = 128) out;

uniform mat4 pv;
uniform bool curve;

in Curve {
    vec3 position0;
    vec3 position1;
    vec3 position2;
    vec3 position3;
    int rank; // in range 1-3
} gs[];

vec4 toScreen(vec3 position) {
    return pv * vec4(position, 1.f);
}

void emitPolyline() {
    vec3 positions[4] = {gs[0].position0, gs[0].position1, gs[0].position2, gs[0].position3};
    for (int i = 0; i <= gs[0].rank; i++){
        gl_Position = toScreen(position[i]);
        EmitVertex();
    }
}

void emitCurve() {
    vec4 p0 = toScreen(gs[0].position0);
    vec4 p1 = toScreen(gs[0].position1);
    vec4 p2 = toScreen(gs[0].position2);
    vec4 p3 = toScreen(gs[0].position3);
    float l = length(p0 - p1) + length(p1 - p2) + length(p2 - p3);
    int subdivisions = clamp((int)(l / 0.01f), 1, 128);

    for (int i = 0; i <= subdivisions; i++) {
        vec3 positions[4] = {gs[0].position0, gs[0].position1, gs[0].position2, gs[0].position3};
        float t = i * 1.f / subdivisions;
        for (int j = 0; j < gs[0].rank; j++)
            for (int k = 0; k < gs[0].rank - j; k++)
                positions[k] = positions[k] * t + positions[k+1] * (1.f-t);
        gl_Position = toScreen(positions[0]);
        EmitVertex();
    }
}

void main() {
    if (curve)
        emitCurve();
    else
        emitPolyline();
    EndPrimitive();
}
