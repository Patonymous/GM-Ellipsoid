#ifndef POINT_H
#define POINT_H

#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>

#include "../common/position_params.h"
#include "../renderable.h"

class Point : public IRenderable {
    Q_OBJECT

    static uint sm_count;

public:
    Point(PVec4 position);
    virtual ~Point();

    void initializeGL() override;
    void paintGL(const Projection &projection, const Camera &camera) override;

    QList<QWidget *> ui() override;

private:
    PositionParams m_positionUi;

    QOpenGLVertexArrayObject m_vao;
    QOpenGLShaderProgram     m_program;
    QOpenGLBuffer            m_vertexBuffer;
    QOpenGLBuffer            m_indexBuffer;
};

#endif // POINT_H
