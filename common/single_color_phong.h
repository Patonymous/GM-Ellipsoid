#ifndef SINGLE_COLOR_PHONG_H
#define SINGLE_COLOR_PHONG_H

#include <QOpenGLShaderProgram>

struct VertexPositionNormal {
    float position[3];
    float normal[3];
};

struct SingleColorPhong {
    // Vertex shader
    static constexpr const char *const VERTEX_SHADER_FILE =
        "common/single_color_phong/vertex_shader.glsl";

    static constexpr const char *const PV              = "pv";
    static constexpr const char *const CAMERA_POSITION = "cameraPosition";
    static constexpr const char *const LIGHT_POSITION  = "lightPosition";
    static constexpr const char *const MATERIAL        = "material";
    static constexpr const char *const VERTEX_POSITION = "vsPosition";
    static constexpr const char *const VERTEX_NORMAL   = "vsNormal";

    // Fragment shader
    static constexpr const char *const FRAGMENT_SHADER_FILE =
        "common/single_color_phong/fragment_shader.glsl";

    static constexpr const char *const MODEL    = "model";
    static constexpr const char *const TI_MODEL = "ti_model";
    static constexpr const char *const COLOR    = "color";

    static inline void loadAndlinkShaders(QOpenGLShaderProgram &program) {
        program.addCacheableShaderFromSourceFile(
            QOpenGLShader::Vertex, VERTEX_SHADER_FILE
        );
        program.addCacheableShaderFromSourceFile(
            QOpenGLShader::Fragment, FRAGMENT_SHADER_FILE
        );
        program.link();
    }

    static inline void prepareAttributeArrays(QOpenGLShaderProgram &program) {
        program.setAttributeBuffer(
            VERTEX_POSITION, GL_FLOAT, offsetof(VertexPositionNormal, position),
            3, sizeof(VertexPositionNormal)
        );
        program.enableAttributeArray(VERTEX_POSITION);
        program.setAttributeBuffer(
            VERTEX_NORMAL, GL_FLOAT, offsetof(VertexPositionNormal, normal), 3,
            sizeof(VertexPositionNormal)
        );
        program.enableAttributeArray(VERTEX_NORMAL);
    }
};

#endif // SINGLE_COLOR_PHONG_H
