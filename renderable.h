#ifndef RENDERABLE_H
#define RENDERABLE_H

#include <QOpenGLDebugLogger>
#include <QOpenGLFunctions>
#include <QOpenGLWidget>
#include <QString>

class OpenGLArea;

class IRenderable : public QObject, protected QOpenGLFunctions {
    Q_OBJECT

public:
    IRenderable() : QObject(nullptr) {}

    virtual QString debugId() const = 0;

    virtual void initializeGL(QOpenGLContext *context) = 0;
    virtual void paintGL(QOpenGLContext *context)      = 0;

signals:
    void needRepaint();
};

class OpenGLArea : public QOpenGLWidget, QOpenGLFunctions {
    Q_OBJECT

    struct PlacedRenderable {
        IRenderable *renderable;
        bool         wasInitialized;

        bool operator==(const IRenderable *other) const {
            return renderable == other;
        }
    };

public:
    OpenGLArea(QWidget *parent);

public slots:
    void tryPlaceRenderable(IRenderable *renderable);
    void ensureUpdatePending();

protected:
    void initializeGL() override;
    void paintGL() override;

private:
    bool m_updatePending;

    QList<PlacedRenderable> m_placed;

    QOpenGLDebugLogger m_logger;
};

#endif // RENDERABLE_H
