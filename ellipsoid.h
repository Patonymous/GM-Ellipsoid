#ifndef ELLIPSOID_INCLUDED
#define ELLIPSOID_INCLUDED

#define DPRINT(x) qDebug() << QTime::currentTime().msecsSinceStartOfDay() << x;
// #define DPRINT(x)

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

#include "pmath.h"

class Ellipsoid;

struct Params {
    // Be very careful when changing the order of members!

    uint width;
    uint height;
    uint pixelGranularity;

    uchar materialRed;
    uchar materialGreen;
    uchar materialBlue;

    float positionX;
    float positionY;
    float positionZ;
    float scale;

    float stretchX;
    float stretchY;
    float stretchZ;

    float cameraAngleX;
    float cameraAngleY;
    float cameraDistance;

    float lightAmbient;
    float lightDiffuse;
    float lightSpecular;
    float lightSpecularFocus;

    inline bool operator==(const Params &other) const {
        return width == other.width && height == other.height
            && pixelGranularity == other.pixelGranularity
            && materialRed == other.materialRed
            && materialGreen == other.materialGreen
            && materialBlue == other.materialBlue
            && pEqual(positionX, other.positionX)
            && pEqual(positionY, other.positionY)
            && pEqual(positionZ, other.positionZ)
            && pEqual(stretchX, other.stretchX)
            && pEqual(stretchY, other.stretchY)
            && pEqual(stretchZ, other.stretchZ)
            && pEqual(cameraAngleX, other.cameraAngleX)
            && pEqual(cameraAngleY, other.cameraAngleY)
            && pEqual(cameraDistance, other.cameraDistance)
            && pEqual(lightAmbient, other.lightAmbient)
            && pEqual(lightDiffuse, other.lightDiffuse)
            && pEqual(lightSpecular, other.lightSpecular)
            && pEqual(lightSpecularFocus, other.lightSpecularFocus);
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
    float lightIntensityAtCastRay(
        float x, float y, float ambient, float diffuse, float specular,
        float specularFocus
    );

    QElapsedTimer m_timer;

    PVec4 m_camera;
    PMat4 m_equation;

    PMat4 m_pvme;
    PMat4 m_pvInverse;

    Ellipsoid *m_ellipsoid;

private slots:
    void renderEllipsoid(Params parms);

signals:
    void renderCompleted();
};

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
