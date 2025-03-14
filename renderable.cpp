#include "renderable.h"
#include "helpers.h"

OpenGLArea::OpenGLArea(QWidget *parent)
    : QOpenGLWidget(parent), m_updatePending(true), m_placed(), m_logger(this) {
    QSurfaceFormat fmt;
    fmt.setVersion(3, 3);
    fmt.setProfile(QSurfaceFormat::CoreProfile);
    fmt.setOption(QSurfaceFormat::DebugContext);
    setFormat(fmt);
}

void OpenGLArea::tryPlaceRenderable(IRenderable *renderable) {
    if (!m_placed.contains(renderable)) {
        m_placed.append({renderable, false});
        QObject::connect(
            renderable, &IRenderable::needRepaint, this,
            &OpenGLArea::ensureUpdatePending
        );
    }
    ensureUpdatePending();
}

void OpenGLArea::initializeGL() {
    initializeOpenGLFunctions();

    m_logger.initialize();
    m_logger.disableMessages(QList<uint>{131185});

    glViewport(0, 0, width(), height());
    glClearColor(0.f, 0.1f, 0.05f, 1.f);
}

void OpenGLArea::paintGL() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    auto glCtx = context();
    for (auto &[renderable, initialized] : m_placed) {
        if (!initialized) {
            renderable->initializeGL(glCtx);
            initialized = true;

            for (const auto &m : m_logger.loggedMessages())
                qDebug() << m;
            DPRINT(renderable->debugId() << "initialized.");
        }

        renderable->paintGL(glCtx);
        for (const auto &m : m_logger.loggedMessages())
            qDebug() << m;
        DPRINT(renderable->debugId() << "painted.");
    }

    m_updatePending = false;
}

void OpenGLArea::ensureUpdatePending() {
    if (m_updatePending)
        return;

    m_updatePending = true;
    update();
}
