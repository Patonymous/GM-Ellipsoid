#include <QLabel>
#include <QOpenGLPixelTransferOptions>

#include "../common/shape_indices.h"
#include "../common/white.h"
#include "../helpers.h"
#include "../pmath.h"
#include "polyline.h"

constexpr uint MAX_SEGMENTS = 32;

struct PolylineSegment {
    int index;
    int rank;
};

uint Polyline::sm_count = 0;

Polyline::Polyline(QList<IRenderable *> controlPoints)
    : IRenderable(
          ObjectType::PolylineObject,
          QString("Polyline_%1").arg(QString::number(++sm_count))
      ),
      m_renameUi(), m_controlPointsOutdated(true),
      m_controlPoints(controlPoints), m_vao(), m_program(),
      m_texture(QOpenGLTexture::Target1D),
      m_vertexBuffer(QOpenGLBuffer::VertexBuffer) {
    setLocks(ScalingLock | TranslationLock | RotationLock);

    m_renameUi.setupConnections(this);

    for (auto r : controlPoints)
        QObject::connect(
            r, &IRenderable::positionChanged, this,
            &Polyline::requestControlPointsUpdate
        );
}
Polyline::~Polyline() {}

void Polyline::initializeGL() {
    initializeOpenGLFunctions();

    m_vao.create();
    m_program.create();
    m_texture.create();
    m_vertexBuffer.create();

    m_program.addCacheableShaderFromSourceFile(
        QOpenGLShader::Vertex, "polyline/vertex_shader.glsl"
    );
    m_program.addCacheableShaderFromSourceFile(
        QOpenGLShader::Geometry, "polyline/geometry_shader.glsl"
    );
    White::loadFragmentShader(m_program);
    m_program.link();

    m_program.bind();
    m_vao.bind();

    m_texture.bind();
    m_texture.setMipLevels(0);
    m_texture.setSize(MAX_SEGMENTS);
    m_texture.setFormat(QOpenGLTexture::RGB32F);
    m_texture.setMinMagFilters(
        QOpenGLTexture::Nearest, QOpenGLTexture::Nearest
    );
    m_texture.allocateStorage(QOpenGLTexture::RGB, QOpenGLTexture::Float32);

    m_program.setUniformValue("controlPoints", m_texture.textureId());

    m_vertexBuffer.bind();
    m_vertexBuffer.setUsagePattern(QOpenGLBuffer::DynamicDraw);
    m_vertexBuffer.allocate(MAX_SEGMENTS * sizeof(PolylineSegment));

    m_program.setAttributeBuffer(
        0, GL_INT, offsetof(PolylineSegment, index), 1, sizeof(PolylineSegment)
    );
    m_program.enableAttributeArray(0);

    m_program.setAttributeBuffer(
        1, GL_INT, offsetof(PolylineSegment, rank), 1, sizeof(PolylineSegment)
    );
    m_program.enableAttributeArray(1);

    m_vao.release();
    m_program.release();
    m_texture.release();
    m_vertexBuffer.release();
}

void Polyline::paintGL(const Projection &projection, const Camera &camera) {
    if (m_controlPointsOutdated) {
        m_controlPointsOutdated = false;

        auto data = new QVector3D[MAX_SEGMENTS];
        for (uint i = 0; i < m_controlPoints.size(); i++) {
            data[i] = (QVector3D)m_controlPoints[i]->model().position;
        }
        m_texture.bind();
        m_texture.setData(QOpenGLTexture::RGB, QOpenGLTexture::Float32, data);
        m_texture.release();
        delete[] data;

        m_vertexBuffer.bind();
        auto segments =
            (PolylineSegment *)m_vertexBuffer.map(QOpenGLBuffer::WriteOnly);
        for (int i = 0; i < segmentCount(); i++) {
            segments[i].index = 1; // 3 * i;
            segments[i].rank =
                1; // qMin((int)m_controlPoints.size() - 3 * i - 1, 3);
        }
        m_vertexBuffer.unmap();
        m_vertexBuffer.release();
    }

    m_program.bind();
    m_program.setUniformValue(
        "pv", (QMatrix4x4)(projection.matrix() * camera.matrix())
    );
    m_program.setUniformValue("curve", false);

    m_texture.bind();
    m_vao.bind();

    // FIXME: Segfault
    glDrawArrays(GL_POINTS, 0, segmentCount());

    m_vao.release();
    m_texture.release();
    m_program.release();
}

QList<QWidget *> Polyline::ui() { return {&m_renameUi}; }

uint Polyline::segmentCount() const { return (m_controlPoints.size() + 1) / 3; }

void Polyline::requestControlPointsUpdate() { m_controlPointsOutdated = true; }

bool Polyline::tryRemoveControlPoint(IRenderable *renderable) {
    auto idx = m_controlPoints.indexOf(renderable);
    if (idx == -1)
        return false;

    QObject::disconnect(
        renderable, &IRenderable::positionChanged, this,
        &Polyline::requestControlPointsUpdate
    );

    m_controlPoints.remove(idx);
    if (m_controlPoints.size() > 1) {
        m_controlPointsOutdated = true;
        emit needRepaint();
    } else {
        emit needRemoval(this);
    }
    return true;
}
