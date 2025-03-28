#include <QLabel>
#include <QOpenGLDebugLogger>

#include "../helpers.h"
#include "../pmath.h"
#include "torus.h"

struct TorusPoint {
    GLfloat tRadian;
    GLfloat sRadian;
};

struct TorusLine {
    GLuint startIdx;
    GLuint endIdx;
};

uint Torus::sm_count = 0;

Torus::Torus(PVec4 position)
    : IRenderable(QString("Torus_%1").arg(QString::number(sm_count))),
      m_name(QString("Torus %1").arg(QString::number(++sm_count))), //
      m_paramsUi(),                                                 //
      m_tSamples(8), m_sSamples(6),                                 //
      m_bigRadius(3.f), m_smallRadius(1.f),                         //
      m_lastTSamples(0), m_lastSSamples(0),                         //
      m_model(
          false, false, false, {1.f, 1.f, 1.f, 0.f}, position,
          {1.f, 0.f, 0.f, 0.f}
      ),
      m_vao(), m_program(), //
      m_paramBuffer(QOpenGLBuffer::VertexBuffer),
      m_indexBuffer(QOpenGLBuffer::IndexBuffer) {
    m_paramsUi.setupConnections(this);
}
Torus::~Torus() {}

void Torus::initializeGL() {
    initializeOpenGLFunctions();

    m_vao.create();
    m_program.create();
    m_paramBuffer.create();
    m_indexBuffer.create();

    m_program.addCacheableShaderFromSourceFile(
        QOpenGLShader::Vertex, "torus/vertex_shader.glsl"
    );
    m_program.addCacheableShaderFromSourceFile(
        QOpenGLShader::Fragment, "torus/fragment_shader.glsl"
    );
    m_program.link();

    m_program.bind();
    m_vao.bind();

    m_paramBuffer.bind();
    m_paramBuffer.setUsagePattern(QOpenGLBuffer::DynamicDraw);
    m_paramBuffer.allocate(16 * 16 * sizeof(TorusPoint));

    m_program.setAttributeBuffer(0, GL_FLOAT, 0, 2);
    m_program.enableAttributeArray(0);

    m_indexBuffer.bind();
    m_indexBuffer.setUsagePattern(QOpenGLBuffer::DynamicDraw);
    m_indexBuffer.allocate(2 * 16 * 16 * sizeof(TorusLine));

    m_vao.release();
    m_program.release();
    m_paramBuffer.release();
    m_indexBuffer.release();
}

void Torus::paintGL(const Projection &projection, const Camera &camera) {
    if (m_tSamples != m_lastTSamples || m_sSamples != m_lastSSamples) {
        DPRINT("Refreshing torus vertex model");

        m_paramBuffer.bind();
        auto pBuffer =
            (TorusPoint *)(m_paramBuffer.map(QOpenGLBuffer::WriteOnly));
        for (uint t = 0; t < m_tSamples; t++) {
            for (uint s = 0; s < m_sSamples; s++) {
                pBuffer[t * m_sSamples + s] = {
                    t * 2.f * PI_F / m_tSamples, s * 2.f * PI_F / m_sSamples
                };
            }
        }
        m_paramBuffer.unmap();
        m_paramBuffer.release();

        m_indexBuffer.bind();
        auto iBuffer =
            (TorusLine *)(m_indexBuffer.map(QOpenGLBuffer::WriteOnly));
        *(iBuffer++) = {(uint)(m_tSamples - 1) * m_sSamples, 0};
        for (uint s = 1; s < m_sSamples; s++) {
            *(iBuffer++) = {s - 1, s};
            *(iBuffer++) = {(m_tSamples - 1) * m_sSamples + s, s};
        }
        *(iBuffer++) = {(uint)m_sSamples - 1, 0};
        for (uint t = 1; t < m_tSamples; t++) {
            *(iBuffer++) = {(t - 1) * m_sSamples, t * m_sSamples};
            for (uint s = 1; s < m_sSamples; s++) {
                *(iBuffer++) = {t * m_sSamples + s - 1, t * m_sSamples + s};
                *(iBuffer++) = {(t - 1) * m_sSamples + s, t * m_sSamples + s};
            }
            *(iBuffer++) = {(t + 1) * m_sSamples - 1, t * m_sSamples};
        }
        m_indexBuffer.unmap();
        m_indexBuffer.release();

        m_lastTSamples = m_tSamples;
        m_lastSSamples = m_sSamples;
    }

    m_program.bind();
    m_program.setUniformValue(
        "pvm",
        (QMatrix4x4)(projection.matrix() * camera.matrix() * m_model.matrix())
    );
    m_program.setUniformValue("radius", QVector2D(m_bigRadius, m_smallRadius));

    m_vao.bind();

    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glDrawElements(
        GL_LINES, 4 * m_tSamples * m_sSamples, GL_UNSIGNED_INT, nullptr
    );
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    m_vao.release();
    m_program.release();
}

QString Torus::type() const { return "Torus"; }

QList<QWidget *> Torus::ui() { return {&m_paramsUi}; }

bool Torus::handleKey(QKeyEvent *event) {
    return m_model.updateByKeyboardControls(event);
}

int Torus::tSamples() const { return m_tSamples; }

int Torus::sSamples() const { return m_sSamples; }

float Torus::bigRadius() const { return m_bigRadius; }

float Torus::smallRadius() const { return m_smallRadius; }

void Torus::setName(QString value) { IRenderable::setName(value); }

void Torus::setTSamples(int value) {
    m_tSamples = value;
    emit needRepaint();
}

void Torus::setSSamples(int value) {
    m_sSamples = value;
    emit needRepaint();
}

void Torus::setBigRadius(double value) {
    m_bigRadius = value;
    emit needRepaint();
}

void Torus::setSmallRadius(double value) {
    m_smallRadius = value;
    emit needRepaint();
}
