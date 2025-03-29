#ifndef CAMERA_H
#define CAMERA_H

#include <QString>

#include "../pmath.h"

struct Camera {
    enum Type {
        Free, // FIXME
        Orbit,
    } type;

    float radianX, radianY;

    PVec4 freePosition;

    float orbitDistance;
    PVec4 orbitCenter;

    Camera(
        Type t, float rx, float ry, const PVec4 &fp, float od, const PVec4 &oc
    )
        : type(t), radianX(rx), radianY(ry), freePosition(fp),
          orbitDistance(od), orbitCenter(oc) {}

    operator QString() const {
        const int angleX = (int)(radianX * 180.f / PI_F);
        const int angleY = (int)(radianY * 180.f / PI_F);
        switch (type) {
        case Free:
            return QString("Free, Pos: %1, X: %2, Y: %3")
                .arg(
                    (QString)freePosition, QString::number(angleX),
                    QString::number(angleY)
                );
        case Orbit:
            return QString("Orbit, Cen: %1, Dis: %2, X: %3, Y: %4")
                .arg(
                    (QString)orbitCenter, QString::number(orbitDistance),
                    QString::number(angleX), QString::number(angleY)
                );
            break;
        }
        return "Error";
    }

    PMat4 matrix() const { return PMat4::lookTo(position(), direction()); }

    PMat4 rotation() const {
        return PMat4::rotationY(radianY) * PMat4::rotationX(radianX);
    }

    PVec4 position() const {
        switch (type) {
        case Free:
            return freePosition;
        case Orbit:
            return rotation() * PVec4{0.f, 0.f, -orbitDistance, 1.f};
        }
        throw std::logic_error("Unknown camera type");
    }

    PVec4 direction() const { return rotation() * PVec4{0.f, 0.f, 1.f, 0.f}; }
};

#endif // CAMERA_H
