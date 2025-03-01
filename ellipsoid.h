#ifndef ELLIPSOID_H
#define ELLIPSOID_H

#include <QOpenGLBuffer>
#include <QOpenGLFunctions>
#include <QOpenGLWidget>

class Ellipsoid : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT

public:
    Ellipsoid(QWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
    ~Ellipsoid();

protected:
    void initializeGL() override;
    void paintGL() override;

private slots:
    void cleanup();

private:
    QOpenGLBuffer m_ellipsoidVbo;
    QList<GLfloat> m_pixelData;
};

#endif // ELLIPSOID_H
