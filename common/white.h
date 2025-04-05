#ifndef WHITE_H
#define WHITE_H

#include <QOpenGLShaderProgram>

struct White {
    // Fragment shader
    static constexpr const char *const FRAGMENT_SHADER_FILE =
        "common/white/fragment_shader.glsl";

    static void loadFragmentShader(QOpenGLShaderProgram &program) {
        program.addCacheableShaderFromSourceFile(
            QOpenGLShader::Fragment, FRAGMENT_SHADER_FILE
        );
    }
};

#endif // WHITE_H
