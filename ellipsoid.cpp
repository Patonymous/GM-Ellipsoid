#include <QTimer>

#include "ellipsoid.h"

constexpr uint COLOR_CHANNELS = 4;
constexpr QOpenGLTexture::PixelFormat PIXEL_FORMAT = QOpenGLTexture::PixelFormat::RGBA;
constexpr QOpenGLTexture::PixelType PIXEL_TYPE = QOpenGLTexture::PixelType::UInt8;
constexpr QOpenGLTexture::Target TEXTURE_TARGET = QOpenGLTexture::Target::Target2D;

GLfloat vertices[] = {
    -1, -1, 0,
    +1, -1, 0,
    +1, +1, 0,
    +1, +1, 0,
    -1, +1, 0,
    -1, -1, 0};

GLfloat textureCoords[] = {
    0, 0,
    1, 0,
    1, 1,
    1, 1,
    0, 1,
    0, 0};

const char *vertexShader =
    "#version 330 core\n"
    "layout (location = 0) in vec3 pos;\n"
    "layout (location = 1) in vec2 tex;\n"
    "\n"
    "out vec2 coord;\n"
    "\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(pos.x, pos.y, pos.z, 1.0f);\n"
    "   coord = tex;\n"
    "}\n";

const char *fragmentShader =
    "#version 330 core\n"
    "uniform sampler2D screen;\n"
    "\n"
    "in vec2 coord;\n"
    "\n"
    "out vec4 fragColor;\n"
    "\n"
    "void main()\n"
    "{\n"
    // "   fragColor = vec4(coord, 0.0f, 0.0f);\n"
    "   fragColor = texture(screen,coord);\n"
    "}\n";

Ellipsoid::Ellipsoid(QWidget *parent, Qt::WindowFlags f)
    : QOpenGLWidget(parent, f),
      m_texture(TEXTURE_TARGET),
      m_pixelGranularity(8),
      m_materialRed(255),
      m_materialGreen(255),
      m_materialBlue(0),
      m_stretchX(1),
      m_stretchY(0),
      m_stretchZ(0),
      m_cameraAngle(0),
      m_cameraDistance(10)
{
    QSurfaceFormat fmt;
    fmt.setVersion(3, 3);
    fmt.setProfile(QSurfaceFormat::CoreProfile);
    fmt.setOption(QSurfaceFormat::DebugContext);
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

    m_logger.initialize();

    glViewport(0, 0, w, h);
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glClearColor(0, 0, 0.5, 1);

    m_pixelData.resize(w * h * COLOR_CHANNELS);

    m_program.create();
    m_vao.create();
    m_texture.create();
    m_quad.create();
    m_tex.create();

    m_program.addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShader);
    m_program.addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShader);
    m_program.link();

    m_program.bind();
    m_vao.bind();

    m_texture.bind();

    m_texture.setSize(w, h);
    m_texture.setMipLevels(0);
    m_texture.setBorderColor(1.f, 1.f, 1.f, 1.f);
    m_texture.setWrapMode(QOpenGLTexture::WrapMode::ClampToBorder);
    m_texture.setFormat(QOpenGLTexture::TextureFormat::RGBA8_UNorm);
    m_texture.setMinificationFilter(QOpenGLTexture::Filter::Nearest);
    m_texture.setMagnificationFilter(QOpenGLTexture::Filter::Nearest);
    m_texture.allocateStorage(PIXEL_FORMAT, PIXEL_TYPE);

    m_program.setUniformValue("screen", 0);

    m_quad.bind();

    m_quad.allocate(vertices, sizeof(vertices));
    m_quad.setUsagePattern(QOpenGLBuffer::UsagePattern::StaticDraw);

    m_program.setAttributeBuffer(0, GL_FLOAT, 0, 3);
    m_program.enableAttributeArray(0);

    m_tex.bind();

    m_tex.allocate(textureCoords, sizeof(textureCoords));
    m_tex.setUsagePattern(QOpenGLBuffer::UsagePattern::StaticDraw);

    m_program.setAttributeBuffer(1, GL_FLOAT, 0, 2);
    m_program.enableAttributeArray(1);

    m_program.release();
    m_vao.release();

    m_quad.release();
    m_tex.release();

    for (const auto &m : m_logger.loggedMessages())
        qDebug() << m;

    QObject::connect(context(), &QOpenGLContext::aboutToBeDestroyed, this, &Ellipsoid::cleanup);
}

void Ellipsoid::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT);

    m_vao.bind();
    m_texture.bind();
    m_program.bind();

    m_texture.setData(PIXEL_FORMAT, PIXEL_TYPE, m_pixelData.constData(), nullptr);

    glDrawArrays(GL_TRIANGLES, 0, 6);

    m_vao.release();
    m_texture.release();
    m_program.release();

    for (const auto &m : m_logger.loggedMessages())
        qDebug() << m;
}

void Ellipsoid::handleRender()
{
    update();

    QTimer::singleShot(1000, [this]
                       { renderEllipsoid(); });
}

void Ellipsoid::cleanup()
{
    makeCurrent();
    m_texture.destroy();
    // m_program.destroy();
    m_vao.destroy();
    m_quad.destroy();
    m_tex.destroy();
    doneCurrent();

    QObject::disconnect(context(), &QOpenGLContext::aboutToBeDestroyed, this, &Ellipsoid::cleanup);
}

void Ellipsoid::renderEllipsoid()
{
    if (m_pixelGranularity < 1)
        m_pixelGranularity = 1;
    if (m_pixelGranularity > 32)
        m_pixelGranularity = 32;

    for (int y = 0; y < height(); y += m_pixelGranularity)
    {
        for (int x = 0; x < width(); x += m_pixelGranularity)
        {
            const auto intensity = castRay(
                (x + m_pixelGranularity / 2.f) / width() * 2 - 1,
                (y + m_pixelGranularity / 2.f) / height() * 2 - 1);

            for (int sy = 0; y + sy < height() && sy < m_pixelGranularity; sy++)
            {
                for (int sx = 0; x + sx < width() && sx < m_pixelGranularity; sx++)
                {
                    m_pixelData[((y + sy) * height() + x + sx) * 4 + 0] = m_materialRed * intensity;
                    m_pixelData[((y + sy) * height() + x + sx) * 4 + 1] = m_materialGreen * intensity;
                    m_pixelData[((y + sy) * height() + x + sx) * 4 + 2] = m_materialBlue * intensity;
                    m_pixelData[((y + sy) * height() + x + sx) * 4 + 3] = 255;
                }
            }
        }
    }

    emit renderCompleted();
}

float Ellipsoid::castRay(float x, float y) // both in range <-1,+1>
{
    float intensity = (x * x + y * y) < 0.25 ? 1 : 0;
    if (intensity < 0)
        intensity = 0;
    else if (intensity > 1)
        intensity = 1;
    return intensity;
}
