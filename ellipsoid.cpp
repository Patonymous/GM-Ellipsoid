#include <QTimer>

#include "ellipsoid.h"

constexpr uint COLOR_CHANNELS = 3;
constexpr QOpenGLTexture::PixelFormat PIXEL_FORMAT = QOpenGLTexture::PixelFormat::RGB;
constexpr QOpenGLTexture::PixelType PIXEL_TYPE = QOpenGLTexture::PixelType::UInt8;
constexpr QOpenGLTexture::Target TEXTURE_TARGET = QOpenGLTexture::Target::Target2D;

GLfloat vertices[] = {
    -1, -1, 0,
    1, -1, 0,
    1, 1, 0,
    -1, 1, 0};

const char *vertexShader =
    "#version 330 core\n"
    "layout (location = 0) in vec3 pos;\n"
    "\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(pos.x, pos.y, pos.z, 1.0f);\n"
    "}\n";

const char *fragmentShader =
    "#version 330 core\n"
    "out vec4 fragColor;\n"
    "\n"
    "void main()\n"
    "{\n"
    "   fragColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);\n"
    "}\n";

Ellipsoid::Ellipsoid(QWidget *parent, Qt::WindowFlags f) : QOpenGLWidget(parent, f),
                                                           m_isDirty(false) /* , m_texture(TEXTURE_TARGET) */, m_pixelData()
{
    QSurfaceFormat fmt;
    fmt.setVersion(3, 3);
    fmt.setProfile(QSurfaceFormat::CoreProfile);
    setFormat(fmt);

    QTimer::singleShot(1000, [this]
                       { renderEllipsoid(); });

    QObject::connect(this, &Ellipsoid::renderCompleted, this, &Ellipsoid::handleRender, Qt::QueuedConnection);
}

Ellipsoid::~Ellipsoid()
{
    cleanup();
}

void Ellipsoid::initializeGL()
{
    auto w = width();
    auto h = height();

    initializeOpenGLFunctions();

    glViewport(0, 0, w, h);
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glClearColor(0, 0.1, 0, 1);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    m_stride = 1;
    while (w > m_stride)
        m_stride <<= 1;
    m_pixelData.resize(m_stride * h * COLOR_CHANNELS);

    // m_texture.create();
    // m_texture.allocateStorage(PIXEL_FORMAT, PIXEL_TYPE);

    m_program.create();
    m_vao.create();
    m_quad.create();

    m_program.addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShader);
    m_program.addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShader);
    m_program.link();

    m_program.bind();
    m_vao.bind();
    m_quad.bind();

    m_quad.allocate(vertices, sizeof(vertices));
    m_quad.setUsagePattern(QOpenGLBuffer::UsagePattern::StaticDraw);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), nullptr);
    m_program.enableAttributeArray(0);

    m_program.release();
    m_vao.release();
    m_quad.release();

    QObject::connect(context(), &QOpenGLContext::aboutToBeDestroyed, this, &Ellipsoid::cleanup);
}

void Ellipsoid::paintGL()
{
    if (!m_isDirty)
        return;

    glClearColor(0, 0, 0.1, 1);
    glClear(GL_COLOR_BUFFER_BIT);

    m_vao.bind();
    m_program.bind();

    glDrawArrays(GL_TRIANGLES, 0, 3);

    m_vao.release();
    m_program.release();

    m_isDirty = false;
}

void Ellipsoid::handleRender()
{
    m_isDirty = true;
    update();

    QTimer::singleShot(1000, [this]
                       { renderEllipsoid(); });
}

void Ellipsoid::cleanup()
{
    makeCurrent();
    // m_texture.destroy();
    // m_program.destroy();
    m_vao.destroy();
    m_quad.destroy();
    doneCurrent();

    QObject::disconnect(context(), &QOpenGLContext::aboutToBeDestroyed, this, &Ellipsoid::cleanup);
}

void Ellipsoid::renderEllipsoid()
{
    auto oldBlue = materialBlue;
    materialBlue = materialGreen;
    materialGreen = materialRed;
    materialRed = oldBlue;

    for (int y = 0; y < height(); y++)
    {
        for (int x = 0; x < width(); x++)
        {
            m_pixelData[(y * m_stride + x) * 3 + 0] = materialRed;
            m_pixelData[(y * m_stride + x) * 3 + 1] = materialGreen;
            m_pixelData[(y * m_stride + x) * 3 + 2] = materialBlue;
        }
    }

    emit renderCompleted();
}
