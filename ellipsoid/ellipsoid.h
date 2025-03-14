#ifndef ELLIPSOID_INCLUDED
#define ELLIPSOID_INCLUDED

#include <QElapsedTimer>
#include <QMouseEvent>
#include <QOpenGLBuffer>
#include <QOpenGLDebugLogger>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLWidget>
#include <QThread>
#include <QTime>

#include "renderer.h"

class Ellipsoid : public QOpenGLWidget, protected QOpenGLFunctions {
    Q_OBJECT

    friend Renderer;

public:
    Ellipsoid(QWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
    ~Ellipsoid();

    uint initialPixelGranularity() const;

    const Params &currentParams() const;

public slots:
    void setStretchX(double value);
    void setStretchY(double value);
    void setStretchZ(double value);

    void setLightAmbient(double value);
    void setLightDiffuse(double value);
    void setLightSpecular(double value);
    void setLightSpecularFocus(double value);

    void setScale(double value);

    void setInitialPixelGranularity(int value);

signals:
    void renderRequested(Params params);

protected:
    void initializeGL() override;
    void paintGL() override;

    void mouseMoveEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

    void wheelEvent(QWheelEvent *event) override;

    void keyPressEvent(QKeyEvent *event) override;

private slots:
    void handleRender();
    void cleanup();

private:
    void requestFreshRenderIfPossible();
    void requestRenderUnsafe();

    QPointF m_lastMousePos;
    uint    m_initialPixelGranularity;

    bool           m_dirty;
    bool           m_renderOngoing;
    Params         m_params;
    Params         m_lastParams;
    Renderer       m_renderer;
    QList<GLubyte> m_pixelData;
    QThread        m_worker;

    QOpenGLDebugLogger       m_logger;
    QOpenGLShaderProgram     m_program;
    QOpenGLVertexArrayObject m_vao;
    QOpenGLTexture           m_texture;
    QOpenGLBuffer            m_quad;
    QOpenGLBuffer            m_tex;
};

#endif // ELLIPSOID_INCLUDED
