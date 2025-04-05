#ifndef RENDERABLE_H
#define RENDERABLE_H

#include <QListWidgetItem>
#include <QOpenGLFunctions>
#include <QString>
#include <QWidget>

#include "object_type.h"
#include "pmath.h"
#include "scene.h"

class IRenderable : public QObject, protected QOpenGLFunctions {
    Q_OBJECT

public:
    IRenderable(ObjectType type, QString debugId);
    virtual ~IRenderable();

    ObjectType       type() const;
    const QString   &debugId() const;
    const QString   &name() const;
    QListWidgetItem *listItem() const;

    virtual void initializeGL() = 0;
    virtual void
    paintGL(const Projection &projection, const Camera &camera) = 0;

    virtual QList<QWidget *> ui() = 0;

    virtual bool handleKey(QKeyEvent *event);

    const Model &model() const;

public slots:
    void setName(const QString &value);

    void setPosition(PVec4 value);
    void setPositionX(float value);
    void setPositionY(float value);
    void setPositionZ(float value);

signals:
    void needRepaint();
    void nameChanged(QString value);

    void positionXChanged(float value);
    void positionYChanged(float value);
    void positionZChanged(float value);

private:
    void updateListItemText() const;

    const ObjectType m_type;
    const QString    m_debugId;
    QString          m_name;

    Model m_model;

    mutable QListWidgetItem m_listItem;
};

#endif // RENDERABLE_H
