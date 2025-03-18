#ifndef RENDERABLE_H
#define RENDERABLE_H

#include <QOpenGLFunctions>
#include <QString>

#include "pmath.h"

class IRenderable : public QObject, protected QOpenGLFunctions {
    Q_OBJECT

public:
    IRenderable() : QObject(nullptr) {}

    virtual QString debugId() const = 0;

    virtual void initializeGL()           = 0;
    virtual void paintGL(const PMat4 &pv) = 0;

signals:
    void needRepaint();
};

#endif // RENDERABLE_H
