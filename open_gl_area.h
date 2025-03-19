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

        PVec4 scale;
        PVec4 position;
        PQuat rotation;

        PlacedRenderable(
            IRenderable *r, const PVec4 &sc, const PVec4 &pos, const PQuat &rot,
            bool init = false
        )
            : renderable(r), wasInitialized(init), scale(sc), position(pos),
              rotation(rot) {}

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
    void setActive(IRenderable *renderable);
    void ensureUpdatePending();

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
    bool      m_updatePending;
    SceneInfo m_scene;

    QPointF m_lastMousePos;

    PlacedRenderable       *m_active;
    QList<PlacedRenderable> m_placed;
    QOpenGLDebugLogger      m_logger;
};

#endif // OPEN_GL_AREA_H
