#ifndef RENDERABLE_H
#define RENDERABLE_H

#include <QListWidgetItem>
#include <QOpenGLFunctions>
#include <QString>
#include <QWidget>

#include "pmath.h"
#include "scene.h"

class IRenderable : public QObject, protected QOpenGLFunctions {
    Q_OBJECT

public:
    IRenderable(QString debugId)
        : QObject(nullptr), m_debugId(debugId), m_name(debugId),
          m_listItem(debugId) {
        DPRINT(debugId << "created");
    }
    virtual ~IRenderable() { DPRINT(m_debugId << "destroyed"); }

    const QString   &debugId() const { return m_debugId; };
    const QString   &name() const { return m_name; };
    QListWidgetItem *listItem() const { return &m_listItem; }

    virtual void initializeGL() = 0;
    virtual void
    paintGL(const Projection &projection, const Camera &camera) = 0;

    virtual QString type() const = 0;

    virtual QList<QWidget *> ui() = 0;

    virtual bool handleKey(QKeyEvent *event) = 0;

signals:
    void nameChanged(QString value);
    void needRepaint();

protected:
    void setName(const QString &value) {
        m_name = value;
        m_listItem.setText(QString("%1: %2").arg(type(), name()));
        emit nameChanged(value);
    }

private:
    const QString m_debugId;

    QString m_name;

    mutable QListWidgetItem m_listItem;
};

#endif // RENDERABLE_H
