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

    int m_stride;
    bool m_isDirty;
    QList<GLubyte> m_pixelData;

    QOpenGLDebugLogger m_logger;
    QOpenGLShaderProgram m_program;
    QOpenGLVertexArrayObject m_vao;
    QOpenGLTexture m_texture;
    QOpenGLBuffer m_quad;
    QOpenGLBuffer m_tex;
    // int m_projMatrixLoc;
    // int m_mvMatrixLoc;
    // int m_normalMatrixLoc;
    // int m_lightPosLoc;
    // QMatrix4x4 m_proj;
    // QMatrix4x4 m_camera;
    // QMatrix4x4 m_world;

    uint pixelGranularity;

    GLubyte materialRed = 255;
    GLubyte materialGreen = 0;
    GLubyte materialBlue = 0;

    float stretchX;
    float stretchY;
    float stretchZ;

    float cameraX;
    float cameraY;
    float cameraZ;
};

#endif // ELLIPSOID_H
