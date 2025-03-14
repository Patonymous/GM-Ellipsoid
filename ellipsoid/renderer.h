#ifndef RENDERER_INCLUDED
#define RENDERER_INCLUDED

#include <QWidget>

#include "../helpers.h"
#include "../pmath.h"

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
            && pEqualF(positionX, other.positionX)
            && pEqualF(positionY, other.positionY)
            && pEqualF(positionZ, other.positionZ)
            && pEqualF(stretchX, other.stretchX)
            && pEqualF(stretchY, other.stretchY)
            && pEqualF(stretchZ, other.stretchZ)
            && pEqualF(cameraAngleX, other.cameraAngleX)
            && pEqualF(cameraAngleY, other.cameraAngleY)
            && pEqualF(cameraDistance, other.cameraDistance)
            && pEqualF(lightAmbient, other.lightAmbient)
            && pEqualF(lightDiffuse, other.lightDiffuse)
            && pEqualF(lightSpecular, other.lightSpecular)
            && pEqualF(lightSpecularFocus, other.lightSpecularFocus);
    }
    inline bool operator!=(const Params &other) const {
        return !((*this) == other);
    }
};

class Ellipsoid;

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

#endif // RENDERER_INCLUDED
