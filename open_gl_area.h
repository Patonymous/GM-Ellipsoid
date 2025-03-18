#ifndef OPEN_GL_AREA_H
#define OPEN_GL_AREA_H

#include <QOpenGLDebugLogger>
#include <QOpenGLFunctions>
#include <QOpenGLWidget>

#include <QMouseEvent>

#include "renderable.h"
#include "scene_info.h"

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

    const SceneInfo &sceneInfo();

public slots:
    void setProjection(SceneInfo::Projection value);

    void tryPlaceRenderable(IRenderable *renderable);
    void ensureUpdatePending();

protected:
    void initializeGL() override;
    void paintGL() override;

    void mouseMoveEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

    void wheelEvent(QWheelEvent *event) override;

    void keyPressEvent(QKeyEvent *event) override;

private:
    bool      m_updatePending;
    SceneInfo m_scene;

    QPointF m_lastMousePos;

    QList<PlacedRenderable> m_placed;
    QOpenGLDebugLogger      m_logger;
};

#endif // OPEN_GL_AREA_H
