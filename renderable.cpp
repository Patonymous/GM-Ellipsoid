#include "renderable.h"

constexpr float OBJECT_MOVEMENT_SPEED = 0.05f;
constexpr float OBJECT_ROTATION_SPEED = PI_F / 36.f;

IRenderable::IRenderable(QString debugId)
    : QObject(nullptr), m_debugId(debugId), m_name(debugId),
      m_listItem(debugId) {
    DPRINT(m_debugId << "created");
}

IRenderable::~IRenderable() { DPRINT(m_debugId << "destroyed"); }

const QString &IRenderable::debugId() const { return m_debugId; };

const QString &IRenderable::name() const { return m_name; };

QListWidgetItem *IRenderable::listItem() const { return &m_listItem; }

bool IRenderable::handleKey(QKeyEvent *event) {
    switch (event->key()) {
    case Qt::Key_R:
        setPositionZ(m_model.position.z - OBJECT_MOVEMENT_SPEED);
        break;
    case Qt::Key_Y:
        setPositionZ(m_model.position.z + OBJECT_MOVEMENT_SPEED);
        break;
    case Qt::Key_T:
        setPositionY(m_model.position.y + OBJECT_MOVEMENT_SPEED);
        break;
    case Qt::Key_G:
        setPositionY(m_model.position.y - OBJECT_MOVEMENT_SPEED);
        break;
    case Qt::Key_F:
        setPositionX(m_model.position.x - OBJECT_MOVEMENT_SPEED);
        break;
    case Qt::Key_H:
        setPositionX(m_model.position.x + OBJECT_MOVEMENT_SPEED);
        break;

    case Qt::Key_U:
        m_model.rotation *=
            PQuat::Rotation(-OBJECT_ROTATION_SPEED, {0.f, 0.f, 1.f});
        break;
    case Qt::Key_O:
        m_model.rotation *=
            PQuat::Rotation(+OBJECT_ROTATION_SPEED, {0.f, 0.f, 1.f});
        break;
    case Qt::Key_I:
        m_model.rotation *=
            PQuat::Rotation(+OBJECT_ROTATION_SPEED, {1.f, 0.f, 0.f});
        break;
    case Qt::Key_K:
        m_model.rotation *=
            PQuat::Rotation(-OBJECT_ROTATION_SPEED, {1.f, 0.f, 0.f});
        break;
    case Qt::Key_J:
        m_model.rotation *=
            PQuat::Rotation(-OBJECT_ROTATION_SPEED, {0.f, 1.f, 0.f});
        break;
    case Qt::Key_L:
        m_model.rotation *=
            PQuat::Rotation(+OBJECT_ROTATION_SPEED, {0.f, 1.f, 0.f});
        break;

    default:
        return false;
    }
    return true;
}

const Model &IRenderable::model() const { return m_model; }

void IRenderable::setPosition(PVec4 value) {
    value.x = qBound(-10.f, value.x, 10.f);
    value.y = qBound(-10.f, value.y, 10.f);
    value.z = qBound(-10.f, value.z, 10.f);
    if (pEqualF(m_model.position.x, value.x)
        && pEqualF(m_model.position.y, value.y)
        && pEqualF(m_model.position.z, value.z))
        return;
    m_model.position = value;
    emit positionXChanged(value.x);
    emit positionYChanged(value.y);
    emit positionZChanged(value.z);
    emit needRepaint();
}

void IRenderable::setPositionX(float value) {
    value = qBound(-10.f, value, 10.f);
    if (pEqualF(m_model.position.x, value))
        return;
    m_model.position.x = value;
    emit positionXChanged(value);
    emit needRepaint();
}

void IRenderable::setPositionY(float value) {
    value = qBound(-10.f, value, 10.f);
    if (pEqualF(m_model.position.y, value))
        return;
    m_model.position.y = value;
    emit positionYChanged(value);
    emit needRepaint();
}

void IRenderable::setPositionZ(float value) {
    value = qBound(-10.f, value, 10.f);
    if (pEqualF(m_model.position.z, value))
        return;
    m_model.position.z = value;
    emit positionZChanged(value);
    emit needRepaint();
}

void IRenderable::setName(const QString &value) {
    m_name = value;
    m_listItem.setText(QString("%1: %2").arg(type(), name()));
    emit nameChanged(value);
}
