#include <QLabel>
#include <QOpenGLDebugLogger>

#include "../common/shape_indices.h"
#include "../common/white.h"
#include "../helpers.h"
#include "../pmath.h"
#include "polyline.h"

constexpr uint MAX_CONTROL_POINTS = 32;

struct PolylineSegment {
    float position0[3];
    float position1[3];
    float position2[3];
    float position3[3];
    int   rank;
};

uint Polyline::sm_count = 0;

Polyline::Polyline(QList<IRenderable *> controlPoints)
    : IRenderable(
          ObjectType::PolylineObject,
          QString("Polyline_%1").arg(QString::number(++sm_count))
      ),
      m_renameUi(), m_controlPoints(controlPoints), m_vao(), m_program(),
      m_vertexBuffer(QOpenGLBuffer::VertexBuffer) {
    setLocks(ScalingLock | TranslationLock | RotationLock);

    m_renameUi.setupConnections(this);
}
Polyline::~Polyline() {}

void Polyline::initializeGL() {
    initializeOpenGLFunctions();

    // TODO: Geometry shader, new vertex attributes layout and so on

    m_vao.create();
    m_program.create();
    m_vertexBuffer.create();

    m_program.addCacheableShaderFromSourceFile(
        QOpenGLShader::Vertex, "polyline/vertex_shader.glsl"
    );
    White::loadFragmentShader(m_program);
    m_program.link();

    m_program.bind();
    m_vao.bind();

    m_vertexBuffer.bind();
    m_vertexBuffer.setUsagePattern(QOpenGLBuffer::DynamicDraw);
    m_vertexBuffer.allocate(MAX_CONTROL_POINTS * sizeof(PolylineSegment));

    m_program.setAttributeBuffer("position", GL_FLOAT, 0, 3);
    m_program.enableAttributeArray("position");

    m_vao.release();
    m_program.release();
    m_vertexBuffer.release();
}

void Polyline::paintGL(const Projection &projection, const Camera &camera) {
    bool outdatedVertices =
        m_previousPositions.size() != m_controlPoints.size();
    if (!outdatedVertices) {
        for (uint i = 0; i < m_controlPoints.size(); i++) {
            auto prev = m_previousPositions[i];
            auto curr = m_controlPoints[i]->model().position;
            if ((outdatedVertices = !prev.equals(curr)))
                break;
        }
    }
    if (outdatedVertices) {
        m_previousPositions.resizeForOverwrite(m_controlPoints.size());
        m_vertexBuffer.bind();
        auto v = (PolylineVertex *)m_vertexBuffer.map(QOpenGLBuffer::WriteOnly);
        for (uint i = 0; i < m_controlPoints.size(); i++) {
            auto position          = m_controlPoints[i]->model().position;
            v[i]                   = {position.x, position.y, position.z};
            m_previousPositions[i] = position;
        }
        m_vertexBuffer.unmap();
        m_vertexBuffer.release();
    }

    m_program.bind();
    m_program.setUniformValue(
        "pv", (QMatrix4x4)(projection.matrix() * camera.matrix())
    );

    m_vao.bind();

    glDrawArrays(GL_LINE_STRIP, 0, m_controlPoints.size());

    m_vao.release();
    m_program.release();
}

QList<QWidget *> Polyline::ui() { return {&m_renameUi}; }

bool Polyline::tryRemoveControlPoint(IRenderable *renderable) {
    auto idx = m_controlPoints.indexOf(renderable);
    if (idx == -1)
        return false;

    m_controlPoints.remove(idx);
    if (m_controlPoints.size() > 1)
        emit needRepaint();
    else
        emit needRemoval(this);
    return true;
}
