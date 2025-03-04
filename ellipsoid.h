#ifndef ELLIPSOID_H
#define ELLIPSOID_H

#include <QMatrix4x4>
#include <QOpenGLBuffer>
#include <QOpenGLDebugLogger>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLFunctions>
#include <QOpenGLTexture>
#include <QOpenGLWidget>
#include <QOpenGLShaderProgram>

class Ellipsoid : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT

public:
    Ellipsoid(QWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
    ~Ellipsoid();

signals:
    void renderCompleted();

protected:
    void initializeGL() override;
    void paintGL() override;

private slots:
    void handleRender();
    void cleanup();

private:
    void renderEllipsoid();
    float castRay(float x, float y);

    QList<GLubyte> m_pixelData;

    QOpenGLDebugLogger m_logger;
    QOpenGLShaderProgram m_program;
    QOpenGLVertexArrayObject m_vao;
    QOpenGLTexture m_texture;
    QOpenGLBuffer m_quad;
    QOpenGLBuffer m_tex;

    uint m_pixelGranularity;

    GLubyte m_materialRed;
    GLubyte m_materialGreen;
    GLubyte m_materialBlue;

    float m_stretchX;
    float m_stretchY;
    float m_stretchZ;

    float m_cameraAngle;
    float m_cameraDistance;
};

#endif // ELLIPSOID_H
