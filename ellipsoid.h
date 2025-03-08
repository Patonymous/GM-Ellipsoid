#ifndef ELLIPSOID_H
#define ELLIPSOID_H

#include <QElapsedTimer>
#include <QOpenGLBuffer>
#include <QOpenGLDebugLogger>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLWidget>

#include "pmath.h"

// TODO:
// [X] struct with render params
// [ ] send params to rendered thread via signal on input
// [ ] render on thread pool
// [ ] send signal to UI when completed
// [ ] draw OpenGL
// [ ] send new params if dirty

class Ellipsoid;

struct Params {
    // Be very careful when changing the order of members!

    uint width;
    uint height;
    uint pixelGranularity;

    GLubyte materialRed;
    GLubyte materialGreen;
    GLubyte materialBlue;

    float stretchX;
    float stretchY;
    float stretchZ;

    float cameraAngle;
    float cameraDistance;

    inline bool operator==(const Params &other) const {
        return width == other.width && height == other.height
            && pixelGranularity == other.pixelGranularity
            && materialRed == other.materialRed
            && materialGreen == other.materialGreen
            && materialBlue == other.materialBlue && stretchX == other.stretchX
            && stretchY == other.stretchY && stretchZ == other.stretchZ
            && cameraAngle == other.cameraAngle
            && cameraDistance == other.cameraDistance;
    }
    inline bool operator!=(const Params &other) const {
        return !((*this) == other);
    }
};

class Renderer : public QObject {
    Q_OBJECT

public:
    Renderer(Ellipsoid *parent);
    ~Renderer();

    void setupConnection();

private:
    float lightIntensityAtCastRay(float x, float y);

    QElapsedTimer m_timer;

    PMat4 m_camera;
    PMat4 m_perspective;
    PMat4 m_pc;
    PMat4 m_pcInverse;

    Ellipsoid *m_ellipsoid;

private slots:
    void renderEllipsoid(Params parms);

signals:
    void renderCompleted(Params params);
};

class Ellipsoid : public QOpenGLWidget, protected QOpenGLFunctions {
    Q_OBJECT

    friend Renderer;

public:
    Ellipsoid(QWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
    ~Ellipsoid();

signals:
    void requestRender(Params params);

protected:
    void initializeGL() override;
    void paintGL() override;

private slots:
    void handleRender(Params params);
    void cleanup();

private:
    void requestRenderIfPossible();

    bool           m_dirty;
    bool           m_renderOngoing;
    Params         m_params;
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

#endif // ELLIPSOID_H
