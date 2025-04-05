#include "renderable.h"

IRenderable::IRenderable(ObjectType type, QString debugId)
    : QObject(nullptr), m_type(type), m_debugId(debugId), m_name(debugId),
      m_model(), m_locks(NoLock), m_listItem(debugId) {
    setName(debugId);
    DPRINT(m_debugId << "created");
}

IRenderable::~IRenderable() { DPRINT(m_debugId << "destroyed"); }

ObjectType IRenderable::type() const { return m_type; }

const QString &IRenderable::debugId() const { return m_debugId; };

const QString &IRenderable::name() const { return m_name; };

QListWidgetItem *IRenderable::listItem() const { return &m_listItem; }

const Model &IRenderable::model() const { return m_model; }

void IRenderable::setName(const QString &value) {
    m_name = value;
    updateListItemText();
    emit nameChanged(value);
}

void IRenderable::setScale(PVec4 value) { m_model.scaling = value; }

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

void IRenderable::setLocks(TransformationLocks locks) { m_locks = locks; }

void IRenderable::apply(const ITransformation &transformation) {
    auto transformed = transformation.transform(m_model);

    if (!(m_locks & ScalingLock))
        m_model.scaling = transformed.scaling;
    if (!(m_locks & TranslationLock))
        setPosition(transformed.position);
    if (!(m_locks & RotationLock))
        m_model.rotation = transformed.rotation;
}

void IRenderable::updateListItemText() const {
    QString typeName;
    switch (type()) {
    case ObjectType::CursorObject:
        typeName = "Cursor";
        break;
    case ObjectType::PointObject:
        typeName = "Point";
        break;
    case ObjectType::TorusObject:
        typeName = "Torus";
        break;
    case ObjectType::PolylineObject:
        typeName = "Polyline";
        break;

    default:
        break;
    }

    m_listItem.setText(QString("%1: %2").arg(typeName, name()));
}
