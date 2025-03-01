#include "ellipsoid.h"

Ellipsoid::Ellipsoid(QWidget *parent, Qt::WindowFlags f) : QOpenGLWidget(parent, f)
{
    QSurfaceFormat fmt;
    fmt.setVersion(3, 2);
    fmt.setDepthBufferSize(24);
    fmt.setProfile(QSurfaceFormat::CoreProfile);
    setFormat(fmt);

    m_pixelData.resize(width() * height());
}

Ellipsoid::~Ellipsoid()
{
    cleanup();
}

void Ellipsoid::initializeGL()
{
    initializeOpenGLFunctions();

    glClearColor(0, 1, 0, 1);

    m_ellipsoidVbo.create();
    m_ellipsoidVbo.bind();
    m_ellipsoidVbo.allocate(m_pixelData.constData(), m_pixelData.count());

    QObject::connect(context(), &QOpenGLContext::aboutToBeDestroyed, this, &Ellipsoid::cleanup);
}

void Ellipsoid::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT);
}

void Ellipsoid::cleanup()
{
    makeCurrent();
    m_ellipsoidVbo.destroy();
    doneCurrent();

    QObject::disconnect(context(), &QOpenGLContext::aboutToBeDestroyed, this, &Ellipsoid::cleanup);
}
