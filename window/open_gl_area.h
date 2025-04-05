#ifndef OPEN_GL_AREA_H
#define OPEN_GL_AREA_H

#include <QOpenGLDebugLogger>
#include <QOpenGLFunctions>
#include <QOpenGLWidget>

#include <QMouseEvent>

#include "../cursor/cursor.h"
#include "../renderable.h"
#include "../scene.h"

class OpenGLArea : public QOpenGLWidget, QOpenGLFunctions {
    Q_OBJECT

    struct PlacedRenderable {
        IRenderable *renderable;
        bool         wasInitialized;

        PlacedRenderable(IRenderable *r, bool init = false)
            : renderable(r), wasInitialized(init) {}

        bool operator==(const IRenderable *other) const {
            return renderable == other;
        }
    };

public:
    OpenGLArea(QWidget *parent);

    const Projection &projection() const;
    const Camera     &camera() const;

public slots:
    bool tryAddRenderable(IRenderable *renderable);
    bool tryRemoveRenderable(IRenderable *renderable);

    void setActive(QList<IRenderable *> renderables);

    void ensureUpdatePending();

signals:
    void projectionChanged(const Projection &value) const;
    void cameraChanged(const Camera &value) const;

    void objectClicked(IRenderable *renderable) const;

protected:
    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int w, int h) override;

    void mouseMoveEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

    void wheelEvent(QWheelEvent *event) override;

    void keyPressEvent(QKeyEvent *event) override;

private:
    Cursor          *findCursor();
    PVec4            findGroupCenter();
    ITransformation *newTransformationForEvent(QKeyEvent *event);

    bool       m_updatePending;
    Projection m_projection;
    Camera     m_camera;

    QPointF m_lastMousePos;
    bool    m_mouseSelectionRequested;
    GLuint  m_stencil;

    Cursor                  m_groupMarker;
    QList<IRenderable *>    m_active;
    QList<PlacedRenderable> m_placed;

    QOpenGLDebugLogger m_logger;
};

#endif // OPEN_GL_AREA_H
