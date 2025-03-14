#ifndef TORUS_H
#define TORUS_H

#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>

#include "../renderable.h"

class Torus : public IRenderable {
    Q_OBJECT

public:
    Torus();
    ~Torus();

    QString debugId() const override;

    void initializeGL(QOpenGLContext *context) override;
    void paintGL(QOpenGLContext *context) override;

    void setTSamples(int value);
    int  tSamples() const;

    void setSSamples(int value);
    int  sSamples() const;

private:
    int m_tSamples;
    int m_sSamples;

    QOpenGLVertexArrayObject m_vao;
    QOpenGLShaderProgram     m_program;
    QOpenGLBuffer            m_paramBuffer;
    QOpenGLBuffer            m_indexBuffer;
};

#endif // TORUS_H
