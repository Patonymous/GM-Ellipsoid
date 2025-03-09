#ifndef ELLIPSOID_H
#define ELLIPSOID_H

// #define DPRINT(x) qDebug() << x
#define DPRINT(x)

#include <QElapsedTimer>
#include <QOpenGLBuffer>
#include <QOpenGLDebugLogger>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLWidget>
#include <QThread>

#include "pmath.h"

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

    float cameraAngleX;
    float cameraAngleY;
    float cameraDistance;

    inline bool operator==(const Params &other) const {
        return width == other.width && height == other.height
            && pixelGranularity == other.pixelGranularity
            && materialRed == other.materialRed
            && materialGreen == other.materialGreen
            && materialBlue == other.materialBlue
            && pEqual(stretchX, other.stretchX)
            && pEqual(stretchY, other.stretchY)
            && pEqual(stretchZ, other.stretchZ)
            && pEqual(cameraAngleX, other.cameraAngleX)
            && pEqual(cameraAngleY, other.cameraAngleY)
            && pEqual(cameraDistance, other.cameraDistance);
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

    PVec4 m_camera;
    PMat4 m_equation;

    PMat4 m_pve;
    PMat4 m_pvInverse;

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

public slots:
    void setStretchX(double value);
    void setStretchY(double value);
    void setStretchZ(double value);

signals:
    void requestRender(Params params);

protected:
    void initializeGL() override;
    void paintGL() override;

private slots:
    void handleRender(Params params);
    void cleanup();

private:
    void requestRenderIfPossible(bool resetPixelGranularity);

    uint m_initialPixelGranularity;

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
