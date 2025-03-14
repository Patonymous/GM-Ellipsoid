#ifndef RENDERABLE_H
#define RENDERABLE_H

#include <QOpenGLFunctions>
#include <QOpenGLWidget>

class IRenderable : public QObject, QOpenGLFunctions {
    Q_OBJECT

public:
    IRenderable() : QObject(nullptr) {}

    void virtual render(QOpenGLContext *context) const = 0;
};

class OpenGLArea : public QOpenGLWidget {
    Q_OBJECT

public slots:
    void render(const IRenderable &renderable);

protected:
    void initializeGL() override;
    void paintGL() override;

private:
    void ensureUpdatePending();

    QList<const IRenderable *> m_toRender;

    bool m_updatePending;
};

#endif // RENDERABLE_H
