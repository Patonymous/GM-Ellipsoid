#include <QLabel>
#include <QOpenGLDebugLogger>

#include "../common/shape_indices.h"
#include "../common/single_color_phong.h"
#include "../helpers.h"
#include "../pmath.h"
#include "point.h"

constexpr float SIDE = 0.2f;

constexpr float AMBIENT  = 0.05f;
constexpr float DIFFUSE  = 0.5f;
constexpr float SPECULAR = 0.7f;
constexpr float FOCUS    = 10.0f;

constexpr QVector3D COLOR = {1.f, 1.f, 1.f};

uint Point::sm_count = 0;

Point::Point(PVec4 position)
    : IRenderable(
          ObjectType::PointObject,
          QString("Point_%1").arg(QString::number(++sm_count))
      ),
      m_renameUi(), m_positionUi(), m_vao(), m_program(),
      m_vertexBuffer(QOpenGLBuffer::VertexBuffer),
      m_indexBuffer(QOpenGLBuffer::IndexBuffer) {
    setPosition(position);
    setLocks(ScalingLock | RotationLock);

    m_renameUi.setupConnections(this);
    m_positionUi.setupConnections(this);
}
Point::~Point() {}

void Point::initializeGL() {
    initializeOpenGLFunctions();

    m_vao.create();
    m_program.create();
    m_vertexBuffer.create();
    m_indexBuffer.create();

    SingleColorPhong::loadAndlinkShaders(m_program);

    m_program.bind();
    m_vao.bind();

    m_vertexBuffer.bind();
    m_vertexBuffer.setUsagePattern(QOpenGLBuffer::StaticDraw);
    m_vertexBuffer.allocate(4 * 6 * sizeof(VertexPositionNormal));

    // clang-format off
    auto v = (VertexPositionNormal *)m_vertexBuffer.map(QOpenGLBuffer::WriteOnly);
    // front
    v[ 0] = {{-SIDE, -SIDE, -SIDE}, {  0,  0, -1}};
    v[ 1] = {{-SIDE, +SIDE, -SIDE}, {  0,  0, -1}};
    v[ 2] = {{+SIDE, +SIDE, -SIDE}, {  0,  0, -1}};
    v[ 3] = {{+SIDE, -SIDE, -SIDE}, {  0,  0, -1}};
    // back
    v[ 4] = {{+SIDE, -SIDE, +SIDE}, {  0,  0, +1}};
    v[ 5] = {{+SIDE, +SIDE, +SIDE}, {  0,  0, +1}};
    v[ 6] = {{-SIDE, +SIDE, +SIDE}, {  0,  0, +1}};
    v[ 7] = {{-SIDE, -SIDE, +SIDE}, {  0,  0, +1}};
    // top
    v[ 8] = {{-SIDE, +SIDE, -SIDE}, {  0, +1,  0}};
    v[ 9] = {{-SIDE, +SIDE, +SIDE}, {  0, +1,  0}};
    v[10] = {{+SIDE, +SIDE, +SIDE}, {  0, +1,  0}};
    v[11] = {{+SIDE, +SIDE, -SIDE}, {  0, +1,  0}};
    // bottom
    v[12] = {{-SIDE, -SIDE, +SIDE}, {  0, -1,  0}};
    v[13] = {{-SIDE, -SIDE, -SIDE}, {  0, -1,  0}};
    v[14] = {{+SIDE, -SIDE, -SIDE}, {  0, -1,  0}};
    v[15] = {{+SIDE, -SIDE, +SIDE}, {  0, -1,  0}};
    // left
    v[16] = {{-SIDE, -SIDE, +SIDE}, { -1,  0,  0}};
    v[17] = {{-SIDE, +SIDE, +SIDE}, { -1,  0,  0}};
    v[18] = {{-SIDE, +SIDE, -SIDE}, { -1,  0,  0}};
    v[19] = {{-SIDE, -SIDE, -SIDE}, { -1,  0,  0}};
    // right
    v[20] = {{+SIDE, -SIDE, -SIDE}, { +1,  0,  0}};
    v[21] = {{+SIDE, +SIDE, -SIDE}, { +1,  0,  0}};
    v[22] = {{+SIDE, +SIDE, +SIDE}, { +1,  0,  0}};
    v[23] = {{+SIDE, -SIDE, +SIDE}, { +1,  0,  0}};
    m_vertexBuffer.unmap();
    // clang-format on

    SingleColorPhong::prepareAttributeArrays(m_program);

    m_indexBuffer.bind();
    m_indexBuffer.setUsagePattern(QOpenGLBuffer::StaticDraw);
    m_indexBuffer.allocate(2 * 6 * sizeof(TriangleIndices));

    // clang-format off
    auto i = (TriangleIndices *)m_indexBuffer.map(QOpenGLBuffer::WriteOnly);
    // front
    i[ 0] = {{ 0,  1,  2}};
    i[ 1] = {{ 0,  2,  3}};
    // back
    i[ 2] = {{ 4,  5,  6}};
    i[ 3] = {{ 4,  6,  7}};
    // top
    i[ 4] = {{ 8,  9, 10}};
    i[ 5] = {{ 8, 10, 11}};
    // bottom
    i[ 6] = {{12, 13, 14}};
    i[ 7] = {{12, 14, 15}};
    // left
    i[ 8] = {{16, 17, 18}};
    i[ 9] = {{16, 18, 19}};
    // right
    i[10] = {{20, 21, 22}};
    i[11] = {{20, 22, 23}};
    m_indexBuffer.unmap();
    // clang-format on

    m_vao.release();
    m_program.release();
    m_vertexBuffer.release();
    m_indexBuffer.release();
}

void Point::paintGL(const Projection &projection, const Camera &camera) {
    m_program.bind();
    m_program.setUniformValue(
        SingleColorPhong::PV,
        (QMatrix4x4)(projection.matrix() * camera.matrix())
    );
    auto c = camera.position();
    m_program.setUniformValue(SingleColorPhong::CAMERA_POSITION, (QVector3D)c);
    m_program.setUniformValue(SingleColorPhong::LIGHT_POSITION, (QVector3D)c);
    m_program.setUniformValue(
        SingleColorPhong::MATERIAL, QVector4D(AMBIENT, DIFFUSE, SPECULAR, FOCUS)
    );

    m_vao.bind();

    const auto m = model().matrix();
    m_program.setUniformValue(SingleColorPhong::MODEL, (QMatrix4x4)m);
    m_program.setUniformValue(
        SingleColorPhong::TI_MODEL, (QMatrix4x4)(m.inverse().transpose())
    );
    m_program.setUniformValue(SingleColorPhong::COLOR, COLOR);

    glDrawElements(GL_TRIANGLES, 2 * 6 * 3, GL_UNSIGNED_INT, nullptr);

    m_vao.release();
    m_program.release();
}

QList<QWidget *> Point::ui() { return {&m_renameUi, &m_positionUi}; }
