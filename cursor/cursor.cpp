#include "cursor.h"
#include "../common/shape_indices.h"
#include "../common/single_color_phong.h"

constexpr float ARM_LENGTH = 0.75f;
constexpr float ARM_WIDTH  = 0.05f;

constexpr float AMBIENT  = 0.05f;
constexpr float DIFFUSE  = 0.5f;
constexpr float SPECULAR = 0.7f;
constexpr float FOCUS    = 10.0f;

constexpr QVector3D XColor = {1.f, 0.f, 0.f};
constexpr QVector3D YColor = {0.f, 1.f, 0.f};
constexpr QVector3D ZColor = {0.f, 0.f, 1.f};

uint Cursor::sm_count = 0;

Cursor::Cursor()
    : IRenderable(
          ObjectType::CursorObject,
          QString("Cursor_%1").arg(QString::number(++sm_count))
      ),
      m_positionUi(), m_screenUi(), //
      m_screenX(0.f), m_screenY(0.f), m_lastScreenX(0.f), m_lastScreenY(0.f),
      m_requestedScreenX(0.f), m_requestedScreenY(0.f),
      m_isScreenMoveRequested(false), m_vao(), m_program(), //
      m_vertexBuffer(QOpenGLBuffer::VertexBuffer),
      m_indexBuffer(QOpenGLBuffer::IndexBuffer) {
    setLocks(ScalingLock | RotationLock);

    m_positionUi.setupConnections(this);
    m_screenUi.setupConnections(this);
}

Cursor::~Cursor() {}

void Cursor::initializeGL() {
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
    v[ 0] = {{-ARM_LENGTH, -ARM_WIDTH, -ARM_WIDTH}, {  0,  0, -1}};
    v[ 1] = {{-ARM_LENGTH, +ARM_WIDTH, -ARM_WIDTH}, {  0,  0, -1}};
    v[ 2] = {{+ARM_LENGTH, +ARM_WIDTH, -ARM_WIDTH}, {  0,  0, -1}};
    v[ 3] = {{+ARM_LENGTH, -ARM_WIDTH, -ARM_WIDTH}, {  0,  0, -1}};
    // back
    v[ 4] = {{+ARM_LENGTH, -ARM_WIDTH, +ARM_WIDTH}, {  0,  0, +1}};
    v[ 5] = {{+ARM_LENGTH, +ARM_WIDTH, +ARM_WIDTH}, {  0,  0, +1}};
    v[ 6] = {{-ARM_LENGTH, +ARM_WIDTH, +ARM_WIDTH}, {  0,  0, +1}};
    v[ 7] = {{-ARM_LENGTH, -ARM_WIDTH, +ARM_WIDTH}, {  0,  0, +1}};
    // top
    v[ 8] = {{-ARM_LENGTH, +ARM_WIDTH, -ARM_WIDTH}, {  0, +1,  0}};
    v[ 9] = {{-ARM_LENGTH, +ARM_WIDTH, +ARM_WIDTH}, {  0, +1,  0}};
    v[10] = {{+ARM_LENGTH, +ARM_WIDTH, +ARM_WIDTH}, {  0, +1,  0}};
    v[11] = {{+ARM_LENGTH, +ARM_WIDTH, -ARM_WIDTH}, {  0, +1,  0}};
    // bottom
    v[12] = {{-ARM_LENGTH, -ARM_WIDTH, +ARM_WIDTH}, {  0, -1,  0}};
    v[13] = {{-ARM_LENGTH, -ARM_WIDTH, -ARM_WIDTH}, {  0, -1,  0}};
    v[14] = {{+ARM_LENGTH, -ARM_WIDTH, -ARM_WIDTH}, {  0, -1,  0}};
    v[15] = {{+ARM_LENGTH, -ARM_WIDTH, +ARM_WIDTH}, {  0, -1,  0}};
    // left
    v[16] = {{-ARM_LENGTH, -ARM_WIDTH, +ARM_WIDTH}, { -1,  0,  0}};
    v[17] = {{-ARM_LENGTH, +ARM_WIDTH, +ARM_WIDTH}, { -1,  0,  0}};
    v[18] = {{-ARM_LENGTH, +ARM_WIDTH, -ARM_WIDTH}, { -1,  0,  0}};
    v[19] = {{-ARM_LENGTH, -ARM_WIDTH, -ARM_WIDTH}, { -1,  0,  0}};
    // right
    v[20] = {{+ARM_LENGTH, -ARM_WIDTH, -ARM_WIDTH}, { +1,  0,  0}};
    v[21] = {{+ARM_LENGTH, +ARM_WIDTH, -ARM_WIDTH}, { +1,  0,  0}};
    v[22] = {{+ARM_LENGTH, +ARM_WIDTH, +ARM_WIDTH}, { +1,  0,  0}};
    v[23] = {{+ARM_LENGTH, -ARM_WIDTH, +ARM_WIDTH}, { +1,  0,  0}};
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

void Cursor::paintGL(const Projection &projection, const Camera &camera) {
    auto screenPosition =
        projection.matrix() * camera.matrix() * model().position;
    if (m_isScreenMoveRequested) {
        m_isScreenMoveRequested = false;
        screenPosition.x        = m_requestedScreenX;
        screenPosition.y        = m_requestedScreenY;
        if (projection.type == Projection::Perspective)
            screenPosition.z = qBound(0.9f, screenPosition.z, 0.975f);
        else
            screenPosition.z = qBound(-0.9f, screenPosition.z, 0.5f);

        setPosition(
            (projection.matrix() * camera.matrix()).inverse() * screenPosition
        );
    }
    if (!pEqualF(screenPosition.x, m_screenX)
        || !pEqualF(screenPosition.y, m_screenY)) {
        m_screenX = screenPosition.x;
        m_screenY = screenPosition.y;
        emit screenPositionChanged(m_screenX, m_screenY);
    }

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

    const auto mx = model().matrix();
    m_program.setUniformValue(SingleColorPhong::MODEL, (QMatrix4x4)mx);
    m_program.setUniformValue(
        SingleColorPhong::TI_MODEL, (QMatrix4x4)(mx.inverse().transpose())
    );
    m_program.setUniformValue(SingleColorPhong::COLOR, XColor);

    glDrawElements(GL_TRIANGLES, 2 * 6 * 3, GL_UNSIGNED_INT, nullptr);

    const auto my = mx * PMat4::rotationZ(PI_F / 2);
    m_program.setUniformValue(SingleColorPhong::MODEL, (QMatrix4x4)my);
    m_program.setUniformValue(
        SingleColorPhong::TI_MODEL, (QMatrix4x4)(my.inverse().transpose())
    );
    m_program.setUniformValue(SingleColorPhong::COLOR, YColor);

    glDrawElements(GL_TRIANGLES, 2 * 6 * 3, GL_UNSIGNED_INT, nullptr);

    const auto mz = mx * PMat4::rotationY(PI_F / 2);
    m_program.setUniformValue(SingleColorPhong::MODEL, (QMatrix4x4)mz);
    m_program.setUniformValue(
        SingleColorPhong::TI_MODEL, (QMatrix4x4)(mz.inverse().transpose())
    );
    m_program.setUniformValue(SingleColorPhong::COLOR, ZColor);

    glDrawElements(GL_TRIANGLES, 2 * 6 * 3, GL_UNSIGNED_INT, nullptr);

    m_vao.release();
    m_program.release();
}

QList<QWidget *> Cursor::ui() { return {&m_positionUi, &m_screenUi}; }

float Cursor::screenX() const { return m_screenX; }
float Cursor::screenY() const { return m_screenY; }

void Cursor::requestScreenPosition(float x, float y) {
    m_requestedScreenX      = x;
    m_requestedScreenY      = y;
    m_isScreenMoveRequested = true;
    emit needRepaint();
}
