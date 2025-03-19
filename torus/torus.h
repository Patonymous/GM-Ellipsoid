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

    void initializeGL() override;
    void paintGL(const PMat4 &pv) override;

    int   tSamples() const;
    int   sSamples() const;
    float bigRadius() const;
    float smallRadius() const;

public slots:
    void setTSamples(int value);
    void setSSamples(int value);
    void setBigRadius(double value);
    void setSmallRadius(double value);

private:
    int m_tSamples;
    int m_sSamples;
    int m_lastTSamples;
    int m_lastSSamples;

    float m_bigRadius;
    float m_smallRadius;

    QOpenGLVertexArrayObject m_vao;
    QOpenGLShaderProgram     m_program;
    QOpenGLBuffer            m_paramBuffer;
    QOpenGLBuffer            m_indexBuffer;
};

#endif // TORUS_H
