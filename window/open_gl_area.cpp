#include "open_gl_area.h"

OpenGLArea::OpenGLArea(QWidget *parent)
    : QOpenGLWidget(parent), m_updatePending(true),
      m_projection(Projection::Perspective, PI_F / 2.f, 1.f, 10.f, 0.1f, 50.f),
      m_camera(
          Camera::Orbit, 10.f, 0.f, 0.f, {0.f, 0.f, 10.f, 1.f},
          {0.f, 0.f, 0.f, 0.f}
      ),
      m_active(nullptr), m_placed(), m_logger(this) {
    QSurfaceFormat fmt;
    fmt.setVersion(3, 3);
    fmt.setProfile(QSurfaceFormat::CoreProfile);
    fmt.setOption(QSurfaceFormat::DebugContext);
    setFormat(fmt);
}

const Projection &OpenGLArea::projection() const { return m_projection; }

const Camera &OpenGLArea::camera() const { return m_camera; }

bool OpenGLArea::tryAddRenderable(IRenderable *renderable) {
    if (m_placed.contains(renderable))
        return false;

    m_placed.append(renderable);

    QObject::connect(
        renderable, &IRenderable::needRepaint, this,
        &OpenGLArea::ensureUpdatePending
    );

    ensureUpdatePending();
    return true;
}

bool OpenGLArea::tryRemoveRenderable(IRenderable *renderable) {
    if (!m_placed.contains(renderable))
        return false;

    m_placed.removeAll(renderable);

    QObject::disconnect(
        renderable, &IRenderable::needRepaint, this,
        &OpenGLArea::ensureUpdatePending
    );

    ensureUpdatePending();
    return true;
}

void OpenGLArea::setActive(IRenderable *renderable) { m_active = renderable; }

void OpenGLArea::ensureUpdatePending() {
    if (m_updatePending)
        return;

    m_updatePending = true;
    update();
}

void OpenGLArea::initializeGL() {
    initializeOpenGLFunctions();

    m_logger.initialize();
    m_logger.disableMessages(QList<uint>{131185});

    m_projection.heightToWidthRatio = height() / (float)width();

    glViewport(0, 0, width(), height());
    glClearColor(0.f, 0.1f, 0.05f, 1.f);
}

void OpenGLArea::paintGL() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    for (auto &[renderable, initialized] : m_placed) {
        if (!initialized) {
            renderable->initializeGL();
            initialized = true;

            for (const auto &m : m_logger.loggedMessages())
                qDebug() << m;
            DPRINT(renderable->debugId() << "initialized.");
        }

        renderable->paintGL(m_projection, m_camera);
        for (const auto &m : m_logger.loggedMessages())
            qDebug() << m;
        DPRINT(
            renderable->debugId() << "aka" << renderable->name() << "painted."
        );
    }

    m_updatePending = false;
}

void OpenGLArea::resizeGL(int w, int h) {
    m_projection.heightToWidthRatio = h / (float)w;
    emit projectionChanged(m_projection);
    glViewport(0, 0, w, h);
    ensureUpdatePending();
}

void OpenGLArea::mouseMoveEvent(QMouseEvent *event) {
    if (!(event->buttons() & Qt::RightButton))
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

    m_camera.radianY += deltaX * PI_F / 1000.f;
    if (m_camera.radianY >= 2.f * PI_F)
        m_camera.radianY -= 2.f * PI_F;
    if (m_camera.radianY < 0.f)
        m_camera.radianY += 2.f * PI_F;

    const auto degrees80  = PI_F * 8.f / 18.f;
    m_camera.radianX     += deltaY * PI_F / 1000.f;
    m_camera.radianX      = qBound(-degrees80, m_camera.radianX, degrees80);

    emit cameraChanged(m_camera);
    ensureUpdatePending();
}

void OpenGLArea::mousePressEvent(QMouseEvent *event) {
    if (event->buttons() != 0)
        m_lastMousePos = event->position();
    setFocus(Qt::FocusReason::MouseFocusReason);
}

void OpenGLArea::mouseReleaseEvent(QMouseEvent *event) {
    if (event->buttons() != 0)
        m_lastMousePos = QPointF{0, 0}; // null
    setFocus(Qt::FocusReason::MouseFocusReason);
}

void OpenGLArea::wheelEvent(QWheelEvent *event) {
    event->accept();

    m_camera.distance += event->angleDelta().y() * 0.01f;
    m_camera.distance  = qBound(5.f, m_camera.distance, 15.f);

    emit cameraChanged(m_camera);
    ensureUpdatePending();
}

void OpenGLArea::keyPressEvent(QKeyEvent *event) {
    const float cameraMovementSpeed = 0.1f;

    int handled = 0;
    if (m_camera.type == Camera::Free) {
        handled ^= 0b0000'0001;
        switch (event->key()) {
        case Qt::Key_Q:
            m_camera.position.z -= cameraMovementSpeed;
            break;
        case Qt::Key_E:
            m_camera.position.z += cameraMovementSpeed;
            break;
        case Qt::Key_W:
            m_camera.position.y += cameraMovementSpeed;
            break;
        case Qt::Key_S:
            m_camera.position.y -= cameraMovementSpeed;
            break;
        case Qt::Key_A:
            m_camera.position.x -= cameraMovementSpeed;
            break;
        case Qt::Key_D:
            m_camera.position.x += cameraMovementSpeed;
            break;

        default:
            handled ^= 0b0000'0001;
        }
        if (handled & 0b0000'0001)
            emit cameraChanged(m_camera);
    }
    if (m_active != nullptr && m_active->handleKey(event)) {
        handled |= 0b0000'0010;
    }
    if (event->key() == Qt::Key_Space) {
        switch (m_projection.type) {
        case Projection::Orthographic:
            m_projection.type = Projection::Perspective;
            break;
        case Projection::Perspective:
            m_projection.type = Projection::Orthographic;
            break;
        }
        emit projectionChanged(m_projection);
        handled |= 0b0000'0100;
    }

    if (handled != 0) {
        event->accept();
        ensureUpdatePending();
    }
}
