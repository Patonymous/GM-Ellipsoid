#include "open_gl_area.h"

OpenGLArea::OpenGLArea(QWidget *parent)
    : QOpenGLWidget(parent), m_updatePending(true),
      m_scene(
          SceneInfo::Perspective, SceneInfo::Orbit, //
          10.f, 10.f, 0.1f, 19.9f,                  //
          10.f, 0.f, 0.f, // radians are wrong, but unused
          {0.f, 6.f, -8.f, 1.f}, {0.f, 0.f, 0.f, 0.f}
      ),
      m_placed(), m_logger(this) {
    QSurfaceFormat fmt;
    fmt.setVersion(3, 3);
    fmt.setProfile(QSurfaceFormat::CoreProfile);
    fmt.setOption(QSurfaceFormat::DebugContext);
    setFormat(fmt);
}

const SceneInfo &OpenGLArea::sceneInfo() { return m_scene; }

void OpenGLArea::setProjection(SceneInfo::Projection value) {
    if (m_scene.projectionType == value)
        return;
    m_scene.projectionType = value;
    ensureUpdatePending();
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

    auto pv = m_scene.projectionMatrix() * m_scene.viewMatrix();
    for (auto &[renderable, initialized] : m_placed) {
        if (!initialized) {
            renderable->initializeGL();
            initialized = true;

            for (const auto &m : m_logger.loggedMessages())
                qDebug() << m;
            DPRINT(renderable->debugId() << "initialized.");
        }

        renderable->paintGL(pv);
        for (const auto &m : m_logger.loggedMessages())
            qDebug() << m;
        DPRINT(renderable->debugId() << "painted.");
    }

    m_updatePending = false;
}

void OpenGLArea::mouseMoveEvent(QMouseEvent *event) {
    if (!(event->buttons() & Qt::LeftButton))
        return;

    event->accept();

    const auto position = event->position();
    if (m_lastMousePos.isNull()) {
        m_lastMousePos = position;
        return;
    }

    const auto deltaX = position.x() - m_lastMousePos.x();
    const auto deltaY = position.y() - m_lastMousePos.y();
    m_lastMousePos    = position;

    m_scene.cameraRadianY += deltaX * PI_F / 1000.f;
    if (m_scene.cameraRadianY >= 2.f * PI_F)
        m_scene.cameraRadianY -= 2.f * PI_F;
    if (m_scene.cameraRadianY < 0.f)
        m_scene.cameraRadianY += 2.f * PI_F;

    const auto degrees80   = PI_F * 8.f / 18.f;
    m_scene.cameraRadianX += deltaY * PI_F / 1000.f;
    m_scene.cameraRadianX =
        qBound(-degrees80, m_scene.cameraRadianX, degrees80);

    ensureUpdatePending();
}

void OpenGLArea::mousePressEvent(QMouseEvent *event) {
    if (event->buttons() & Qt::LeftButton)
        m_lastMousePos = event->position();
}

void OpenGLArea::mouseReleaseEvent(QMouseEvent *event) {
    if (event->buttons() & Qt::LeftButton)
        m_lastMousePos = QPointF{0, 0}; // null
    setFocus(Qt::FocusReason::MouseFocusReason);
}

void OpenGLArea::wheelEvent(QWheelEvent *event) {
    event->accept();

    m_scene.cameraDistance += event->angleDelta().y() * 0.01f;
    m_scene.cameraDistance  = qBound(5.f, m_scene.cameraDistance, 15.f);

    ensureUpdatePending();
}

void OpenGLArea::keyPressEvent(QKeyEvent *event) {
    if (m_scene.cameraType != SceneInfo::Free)
        return;

    switch (event->key()) {
    case Qt::UpArrow:
    case Qt::Key_W:
        m_scene.cameraPosition.y = qMin(m_scene.cameraPosition.y + 0.1f, 3.f);
        break;
    case Qt::DownArrow:
    case Qt::Key_S:
        m_scene.cameraPosition.y = qMax(m_scene.cameraPosition.y - 0.1f, -3.f);
        break;
    case Qt::LeftArrow:
    case Qt::Key_A:
        m_scene.cameraPosition.x = qMax(m_scene.cameraPosition.x - 0.1f, -3.f);
        break;
    case Qt::RightArrow:
    case Qt::Key_D:
        m_scene.cameraPosition.x = qMin(m_scene.cameraPosition.x + 0.1f, 3.f);
        break;
    default:
        return;
    }

    event->accept();
    ensureUpdatePending();
}

void OpenGLArea::ensureUpdatePending() {
    if (m_updatePending)
        return;

    m_updatePending = true;
    update();
}
