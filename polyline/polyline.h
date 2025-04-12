#ifndef POLYLINE_H
#define POLYLINE_H

#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QOpenGLVertexArrayObject>

#include "../common/rename_ui.h"
#include "../renderable.h"

class Polyline : public IRenderable {
    Q_OBJECT

    static uint sm_count;

public:
    Polyline(QList<IRenderable *> controlPoints);
    virtual ~Polyline();

    void initializeGL() override;
    void paintGL(const Projection &projection, const Camera &camera) override;

    QList<QWidget *> ui() override;

    uint segmentCount() const;

public slots:
    void requestControlPointsUpdate();
    bool tryRemoveControlPoint(IRenderable *controlPoint);

signals:
    void needRemoval(IRenderable *renderable);

private:
    RenameUi m_renameUi;

    bool                 m_controlPointsOutdated;
    QList<IRenderable *> m_controlPoints;

    QOpenGLVertexArrayObject m_vao;
    QOpenGLShaderProgram     m_program;
    QOpenGLTexture           m_texture;
    QOpenGLBuffer            m_vertexBuffer;
};

#endif // POLYLINE_H
