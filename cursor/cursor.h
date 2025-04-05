#ifndef CURSOR_H
#define CURSOR_H

#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>

#include "../common/position_params.h"
#include "../renderable.h"
#include "screen_position_params.h"

class Cursor : public IRenderable {
    Q_OBJECT

    static uint sm_count;

public:
    Cursor();
    virtual ~Cursor();

    void initializeGL() override;
    void paintGL(const Projection &projection, const Camera &camera) override;

    QList<QWidget *> ui() override;

    float screenX() const;
    float screenY() const;

public slots:
    void requestScreenPosition(float x, float y);

signals:
    void screenPositionChanged(float x, float y);

private:
    PositionParams       m_positionUi;
    ScreenPositionParams m_screenUi;

    float m_screenX;
    float m_screenY;
    float m_lastScreenX;
    float m_lastScreenY;

    float m_requestedScreenX;
    float m_requestedScreenY;
    bool  m_isScreenMoveRequested;

    QOpenGLVertexArrayObject m_vao;
    QOpenGLShaderProgram     m_program;
    QOpenGLBuffer            m_vertexBuffer;
    QOpenGLBuffer            m_indexBuffer;
};

#endif // CURSOR_H
