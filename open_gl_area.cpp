#include "open_gl_area.h"

OpenGLArea::OpenGLArea(QWidget *parent)
    : QOpenGLWidget(parent), m_updatePending(true),
      m_scene(
          SceneInfo::Perspective, SceneInfo::Orbit, //
          10.f, 10.f, 0.1f, 50.f,                   //
          10.f, 0.f, 0.f, {0.f, 0.f, 10.f, 1.f}, {0.f, 0.f, 0.f, 0.f}
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
        m_placed.append({
            renderable, {0.f, 0.f, 0.f},
             {1.f, 0.f, 0.f, 0.f}
        });
        QObject::connect(
            renderable, &IRenderable::needRepaint, this,
            &OpenGLArea::ensureUpdatePending
        );
    }
    ensureUpdatePending();
}

void OpenGLArea::setActive(IRenderable *renderable) {
    if (renderable == nullptr) {
        m_active = nullptr;
        return;
    }

    const auto index = m_placed.indexOf(renderable);
    if (index == -1)
        throw std::logic_error("Renderable is not placed on the area");
    m_active = &m_placed[index];
}

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

    glViewport(0, 0, width(), height());
    glClearColor(0.f, 0.1f, 0.05f, 1.f);
}

void OpenGLArea::paintGL() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    const auto pv = m_scene.projectionMatrix() * m_scene.viewMatrix();
    for (auto &[renderable, initialized, pos, rot] : m_placed) {
        if (!initialized) {
            renderable->initializeGL();
            initialized = true;

            for (const auto &m : m_logger.loggedMessages())
                qDebug() << m;
            DPRINT(renderable->debugId() << "initialized.");
        }

        const auto model = PMat4::translation(pos) * rot.asMatrix();
        renderable->paintGL(pv * model);
        for (const auto &m : m_logger.loggedMessages())
            qDebug() << m;
        DPRINT(renderable->debugId() << "painted.");
    }

    m_updatePending = false;
}

void OpenGLArea::resizeGL(int w, int h) {
    m_scene.width  = w;
    m_scene.height = h;
    glViewport(0, 0, w, h);
    ensureUpdatePending();
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
    const float cameraMovementSpeed = 0.1f;
    const float objectMovementSpeed = 0.05f;
    const float objectRotationSpeed = PI_F / 36.f;

    bool handled = false;
    if (m_scene.cameraType == SceneInfo::Free) {
        handled = true;
        switch (event->key()) {
        case Qt::Key_W:
            m_scene.cameraPosition.y += cameraMovementSpeed;
            break;
        case Qt::Key_S:
            m_scene.cameraPosition.y -= cameraMovementSpeed;
            break;
        case Qt::Key_A:
            m_scene.cameraPosition.x -= cameraMovementSpeed;
            break;
        case Qt::Key_D:
            m_scene.cameraPosition.x += cameraMovementSpeed;
            break;

        default:
            handled = false;
        }
    }
    if (m_active != nullptr) {
        handled = true;
        switch (event->key()) {
        case Qt::Key_R:
            m_active->position.z -= objectMovementSpeed;
            break;
        case Qt::Key_Y:
            m_active->position.z += objectMovementSpeed;
            break;
        case Qt::Key_T:
            m_active->position.y += objectMovementSpeed;
            break;
        case Qt::Key_G:
            m_active->position.y -= objectMovementSpeed;
            break;
        case Qt::Key_F:
            m_active->position.x -= objectMovementSpeed;
            break;
        case Qt::Key_H:
            m_active->position.x += objectMovementSpeed;
            break;

        case Qt::Key_U:
            m_active->rotation *=
                PQuat::Rotation(-objectRotationSpeed, {0.f, 1.f, 0.f});
            break;
        case Qt::Key_O:
            m_active->rotation *=
                PQuat::Rotation(+objectRotationSpeed, {0.f, 1.f, 0.f});
            break;
        case Qt::Key_I:
            m_active->rotation *=
                PQuat::Rotation(+objectRotationSpeed, {1.f, 0.f, 0.f});
            break;
        case Qt::Key_K:
            m_active->rotation *=
                PQuat::Rotation(-objectRotationSpeed, {1.f, 0.f, 0.f});
            break;
        case Qt::Key_J:
            m_active->rotation *=
                PQuat::Rotation(-objectRotationSpeed, {0.f, 0.f, 1.f});
            break;
        case Qt::Key_L:
            m_active->rotation *=
                PQuat::Rotation(+objectRotationSpeed, {0.f, 0.f, 1.f});
            break;

        default:
            handled = false;
        }
    }

    if (handled) {
        event->accept();
        ensureUpdatePending();
    }
}
