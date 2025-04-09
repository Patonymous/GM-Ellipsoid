#include "open_gl_area.h"
#include "../cursor/cursor.h"

// I don't know why, but stencil is always 1.5 times bigger than viewport
// but only on Windows, not on Linux! (MaxOS untested...)
constexpr float MAGICAL_STENCIL_SCALE =
#if defined(Q_OS_LINUX)
    1.f;
#elif defined(Q_OS_WIN)
    1.5f;
#else
    1.f;
#endif

constexpr int MOUSE_CLICK_TOLERANCE = 20;

constexpr float OBJECT_SCALE_UP_SPEED   = 1.1f;
constexpr float OBJECT_SCALE_DOWN_SPEED = 0.909090909f;
constexpr float OBJECT_MOVEMENT_SPEED   = 0.05f;
constexpr float OBJECT_ROTATION_SPEED   = PI_F / 36.f;

OpenGLArea::OpenGLArea(QWidget *parent)
    : QOpenGLWidget(parent), m_updatePending(true),
      m_projection(Projection::Perspective, PI_F / 2.f, 1.f, 10.f, 0.1f, 50.f),
      m_camera(
          Camera::Orbit, 0.f, 0.f, {0.f, 0.f, 10.f, 1.f}, 10.f,
          {0.f, 0.f, 0.f, 1.f}
      ),
      m_mouseSelectionRequested(false), m_lastMousePos(0, 0), //
      m_groupMarker({0.5f, 0.5f, 0.5f}, true),                //
      m_active(), m_placed(), m_logger(this)                  //
{
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

    m_active.removeAll(renderable);
    m_placed.removeAll(renderable);

    QObject::disconnect(
        renderable, &IRenderable::needRepaint, this,
        &OpenGLArea::ensureUpdatePending
    );

    ensureUpdatePending();
    return true;
}

void OpenGLArea::setActive(QList<IRenderable *> renderables) {
    m_active = renderables;
    ensureUpdatePending();
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

    m_projection.heightToWidthRatio = height() / (float)width();

    glViewport(0, 0, width(), height());
    glClearColor(0.f, 0.1f, 0.05f, 1.f);
    glFrontFace(GL_CW);

    glEnable(GL_STENCIL_TEST);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

    m_groupMarker.initializeGL();

    for (const auto &m : m_logger.loggedMessages())
        qDebug() << m;
    DPRINT("OpenGLArea initialized.");
}

void OpenGLArea::paintGL() {
    glClearStencil(-1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    for (uint i = 0; i < m_placed.size(); i++) {
        auto &[renderable, initialized] = m_placed[i];
        if (!initialized) {
            renderable->initializeGL();
            initialized = true;

            for (const auto &m : m_logger.loggedMessages())
                qDebug() << m;
            DPRINT(renderable->debugId() << "initialized.");
        }

        glStencilFunc(GL_ALWAYS, (GLint)i, 0xFF);
        renderable->paintGL(m_projection, m_camera);

        for (const auto &m : m_logger.loggedMessages())
            qDebug() << m;
        DPRINT(
            renderable->debugId() << "aka" << renderable->name() << "painted."
        );
    }

    glStencilFunc(GL_ALWAYS, -1, 0xFF);

    if (m_active.size() > 1) {
        m_groupMarker.setPosition(findGroupCenter());
        m_groupMarker.paintGL(m_projection, m_camera);
        for (const auto &m : m_logger.loggedMessages())
            qDebug() << m;
        DPRINT("Group marker painted.");
    }

    m_updatePending = false;

    if (m_mouseSelectionRequested) {
        m_mouseSelectionRequested = false;

        uint side    = MOUSE_CLICK_TOLERANCE * MAGICAL_STENCIL_SCALE;
        uint w       = width() * MAGICAL_STENCIL_SCALE;
        uint h       = height() * MAGICAL_STENCIL_SCALE;
        uint centerX = m_lastMousePos.x() * MAGICAL_STENCIL_SCALE;
        uint centerY = h - m_lastMousePos.y() * MAGICAL_STENCIL_SCALE;
        uint left    = qMax(centerX - side, 0U);
        uint right   = qMin(centerX + side, w);
        uint top     = qMin(centerY + side, h);
        uint bottom  = qMax(centerY - side, 0U);

        w = right - left + 1;
        h = top - bottom + 1;

        glPixelStorei(GL_PACK_ALIGNMENT, 1);
        GLuint *stencil = new GLuint[w * h];
        glReadPixels(
            left, bottom, w, h, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, stencil
        );

        left        -= centerX;
        right       -= centerX;
        top         -= centerY;
        bottom      -= centerY;
        auto radius  = qMax(qMax(-left, right), qMax(top, -bottom));

        // a spiral walk beginning at clicked point
        // first found object is reported
        // int x = centerX, y = centerY;
        int  x = -left, y = -bottom;
        bool direction = false;
        uint idx       = 255;
        for (int s = 0; s <= radius; s++) {
            int dx    = direction ? +1 : -1;
            int dy    = direction ? +1 : -1;
            direction = !direction;

            if (x >= 0 && x < w && y >= 0 && y < h) {
                idx = stencil[y * w + x] & 0xFF;
                if (idx != 0xFF)
                    break;
            }
            for (uint i = 0; i < s; i++) {
                x += dx;
                if (x >= 0 && x < w && y >= 0 && y < h) {
                    idx = stencil[y * w + x] & 0xFF;
                    if (idx != 0xFF)
                        break;
                }
            }
            if (idx != 0xFF)
                break;
            for (uint i = 0; i < s; i++) {
                y += dy;
                if (x >= 0 && x < w && y >= 0 && y < h) {
                    idx = stencil[y * w + x] & 0xFF;
                    if (idx != 0xFF)
                        break;
                }
            }
            if (idx != 0xFF)
                break;
            y += dy;
        }
        if (idx != 0xFF)
            emit objectClicked(m_placed[idx].renderable);
        else
            emit objectClicked(nullptr);

        delete[] stencil;
    }
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
    m_lastMousePos = event->position();
    setFocus(Qt::FocusReason::MouseFocusReason);
}

void OpenGLArea::mouseReleaseEvent(QMouseEvent *event) {
    m_lastMousePos = event->position();
    setFocus(Qt::FocusReason::MouseFocusReason);

    switch (event->button()) {
    case Qt::LeftButton:
        m_mouseSelectionRequested = true;
        ensureUpdatePending();
        break;
    case Qt::MiddleButton:
        findCursor()->requestScreenPosition(
            (m_lastMousePos.x() / width()) * 2.f - 1.f,
            1.f - (m_lastMousePos.y() / height()) * 2.f
        );
        break;
    default:
        break;
    }
}

void OpenGLArea::mouseDoubleClickEvent(QMouseEvent *event) {
    if (event->button() == Qt ::RightButton)
        findCursor()->requestScreenPosition(
            (m_lastMousePos.x() / width()) * 2.f - 1.f,
            1.f - (m_lastMousePos.y() / height()) * 2.f
        );
}

void OpenGLArea::wheelEvent(QWheelEvent *event) {
    event->accept();

    if (m_camera.type == Camera::Orbit) {
        m_camera.orbitDistance += event->angleDelta().y() * 0.01f;
        m_camera.orbitDistance  = qBound(1.f, m_camera.orbitDistance, 20.f);

        emit cameraChanged(m_camera);
        ensureUpdatePending();
    }
}

void OpenGLArea::keyPressEvent(QKeyEvent *event) {
    const float cameraMovementSpeed = 0.1f;

    int handled = 0;
    if (m_active.size() != 0) {
        auto transformation = newTransformationForEvent(event);
        if (transformation != nullptr) {
            for (auto r : m_active)
                r->apply(*transformation);
            delete transformation;
            handled |= 0b0000'0100;
        }
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
        handled |= 0b0000'1000;
    }

    if (handled != 0) {
        event->accept();
        ensureUpdatePending();
    }
}

Cursor *OpenGLArea::findCursor() {
    for (int idx = 0; idx < m_placed.size(); idx++)
        if (m_placed[idx].renderable->type() == ObjectType::CursorObject)
            return dynamic_cast<Cursor *>(m_placed[idx].renderable);
    throw std::logic_error("Cursor not found");
}

PVec4 OpenGLArea::findGroupCenter() {
    PVec4 center = {0.f, 0.f, 0.f};
    if (m_active.size() == 0)
        return center;

    for (auto r : m_active)
        center += r->model().position;
    return center / m_active.size();
}

ITransformation *OpenGLArea::newTransformationForEvent(QKeyEvent *event) {
    PVec4 center = (event->modifiers() & Qt::KeyboardModifier::AltModifier)
                     ? findCursor()->model().position
                     : findGroupCenter();
    switch (event->key()) {
    case Qt::Key_Q:
        return new Scaling({1.f, 1.f, OBJECT_SCALE_DOWN_SPEED}, center);
    case Qt::Key_E:
        return new Scaling({1.f, 1.f, OBJECT_SCALE_UP_SPEED}, center);
    case Qt::Key_W:
        return new Scaling({1.f, OBJECT_SCALE_UP_SPEED, 1.f}, center);
    case Qt::Key_S:
        return new Scaling({1.f, OBJECT_SCALE_DOWN_SPEED, 1.f}, center);
    case Qt::Key_A:
        return new Scaling({OBJECT_SCALE_DOWN_SPEED, 1.f, 1.f}, center);
    case Qt::Key_D:
        return new Scaling({OBJECT_SCALE_UP_SPEED, 1.f, 1.f}, center);

    case Qt::Key_R:
        return new Translation({0.f, 0.f, -OBJECT_MOVEMENT_SPEED});
    case Qt::Key_Y:
        return new Translation({0.f, 0.f, +OBJECT_MOVEMENT_SPEED});
    case Qt::Key_T:
        return new Translation({0.f, +OBJECT_MOVEMENT_SPEED, 0.f});
    case Qt::Key_G:
        return new Translation({0.f, -OBJECT_MOVEMENT_SPEED, 0.f});
    case Qt::Key_F:
        return new Translation({-OBJECT_MOVEMENT_SPEED, 0.f, 0.f});
    case Qt::Key_H:
        return new Translation({+OBJECT_MOVEMENT_SPEED, 0.f, 0.f});

    case Qt::Key_U:
        return new Rotation(
            PQuat::rotation(-OBJECT_ROTATION_SPEED, {0.f, 0.f, 1.f}), center
        );
    case Qt::Key_O:
        return new Rotation(
            PQuat::rotation(+OBJECT_ROTATION_SPEED, {0.f, 0.f, 1.f}), center
        );
    case Qt::Key_I:
        return new Rotation(
            PQuat::rotation(+OBJECT_ROTATION_SPEED, {0.f, 1.f, 0.f}), center
        );
    case Qt::Key_K:
        return new Rotation(
            PQuat::rotation(-OBJECT_ROTATION_SPEED, {0.f, 1.f, 0.f}), center
        );
    case Qt::Key_J:
        return new Rotation(
            PQuat::rotation(-OBJECT_ROTATION_SPEED, {1.f, 0.f, 0.f}), center
        );
    case Qt::Key_L:
        return new Rotation(
            PQuat::rotation(+OBJECT_ROTATION_SPEED, {1.f, 0.f, 0.f}), center
        );
    }
    return nullptr;
}
