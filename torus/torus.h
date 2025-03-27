#ifndef TORUS_H
#define TORUS_H

#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>

#include "../renderable.h"
#include "torus_params.h"

class Torus : public IRenderable {
    Q_OBJECT

    static uint sm_count;

public:
    Torus(PVec4 position);
    virtual ~Torus();

    void initializeGL() override;
    void paintGL(const Projection &projection, const Camera &camera) override;

    QString type() const override;

    QList<QWidget *> ui() override;

    bool handleKey(QKeyEvent *event) override;

    int   tSamples() const;
    int   sSamples() const;
    float bigRadius() const;
    float smallRadius() const;

public slots:
    void setName(QString name);

    void setTSamples(int value);
    void setSSamples(int value);
    void setBigRadius(double value);
    void setSmallRadius(double value);

private:
    QString m_name;

    TorusParams m_paramsUi;

    int   m_tSamples;
    int   m_sSamples;
    float m_bigRadius;
    float m_smallRadius;

    int m_lastTSamples;
    int m_lastSSamples;

    Model m_model;

    QOpenGLVertexArrayObject m_vao;
    QOpenGLShaderProgram     m_program;
    QOpenGLBuffer            m_paramBuffer;
    QOpenGLBuffer            m_indexBuffer;
};

#endif // TORUS_H
