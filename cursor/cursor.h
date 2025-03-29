#ifndef CURSOR_H
#define CURSOR_H

#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>

#include "../common/position_params.h"
#include "../renderable.h"

class Cursor : public IRenderable {
    Q_OBJECT

    static uint sm_count;

public:
    Cursor();
    virtual ~Cursor();

    void initializeGL() override;
    void paintGL(const Projection &projection, const Camera &camera) override;

    QString type() const override;

    QList<QWidget *> ui() override;

private:
    PositionParams m_positionUi;

    QOpenGLVertexArrayObject m_vao;
    QOpenGLShaderProgram     m_program;
    QOpenGLBuffer            m_vertexBuffer;
    QOpenGLBuffer            m_indexBuffer;
};

#endif // CURSOR_H
