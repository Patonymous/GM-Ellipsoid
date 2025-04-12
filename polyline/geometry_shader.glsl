#version 330 core

layout (points) in;
layout (line_strip, max_vertices = 128) out;

uniform mat4 pv;
uniform bool curve;
uniform sampler1D controlPoints;

in Segment {
    int index;
    int rank; // in range 1-3
} gs[];

vec3 pointAt(int index) {
    return texture(controlPoints, gs[0].index + index).rgb;
}

vec4 toScreen(vec3 position) {
    return pv * vec4(position, 1.f);
}

void emitPolyline() {
    // for (int i = 0; i <= gs[0].rank; i++){
    //     gl_Position = toScreen(pointAt(i));
    //     EmitVertex();
    // }
    gl_Position = vec4(1.f, gs[0].index, 0.f, 1.f);
    EmitVertex();
    gl_Position = vec4(-0.9f, 0.f, 0.f, 1.f);
    EmitVertex();
    gl_Position = vec4(0.f, -0.5f, 0.f, 1.f);
    EmitVertex();
    gl_Position = vec4(gs[0].rank, 1.f, 0.f, 1.f);
    EmitVertex();
}

void emitCurve() {
    float l = 0.f;
    vec4 prev = toScreen(pointAt(0));
    for (int i = 1; i <= gs[0].rank; i++) {
        vec4 now = toScreen(pointAt(i));
        l += length(now.xy - prev.xy);
        prev = now;
    }
    int subdivisions = clamp(int(l / 0.01f), 1, 128);

    for (int i = 0; i <= subdivisions; i++) {
        float t = i * 1.f / subdivisions;

        vec3 positions[4];
        for (int j = 0; j < gs[0].rank; j++)
            positions[j] = pointAt(j);
            
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
