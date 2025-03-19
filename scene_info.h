#ifndef SCENE_INFO_H
#define SCENE_INFO_H

#include <QString>

#include "pmath.h"

struct Projection {
    enum Type {
        Orthographic,
        Perspective,
    } type;

    float fieldOfView, heightToWidthRatio;
    float width, near, far;

    Projection(Type t, float fov, float hwr, float w, float n, float f)
        : type(t), fieldOfView(fov), heightToWidthRatio(hwr), width(w), near(n),
          far(f) {}

    operator QString() const {
        const int fovAngle = (int)(fieldOfView * 180.f / PI_F);
        switch (type) {
        case Orthographic:
            return QString("Orthographic, W: %1, H/W: %2")
                .arg(
                    QString::number(width), QString::number(heightToWidthRatio)
                );
        case Perspective:
            return QString("Perspective, FOV: %1, H/W: %2")
                .arg(
                    QString::number(fovAngle),
                    QString::number(heightToWidthRatio)
                );
            break;
        }
        return "Error";
    }

    PMat4 matrix() const {
        switch (type) {
        case Orthographic:
            return PMat4::orthographic(
                width, heightToWidthRatio * width, near, far
            );

        case Perspective:
            return PMat4::perspective(
                heightToWidthRatio, fieldOfView, near, far
            );
        }
        throw new std::logic_error("Unknown projection type");
    }
};

struct Camera {
    enum Type {
        Free, // FIXME
        Orbit,
    } type;

    float distance, radianX, radianY;
    PVec4 position, target;

    Camera(Type t, float d, float rx, float ry, const PVec4 &p, const PVec4 &tg)
        : type(t), distance(d), radianX(rx), radianY(ry), position(p),
          target(tg) {}

    operator QString() const {
        const int angleX = (int)(radianX * 180.f / PI_F);
        const int angleY = (int)(radianY * 180.f / PI_F);
        switch (type) {
        case Free:
            return QString("Free, Pos: %1, X: %2, Y: %3")
                .arg(
                    (QString)position, QString::number(angleX),
                    QString::number(angleY)
                );
        case Orbit:
            return QString("Orbit, Dis: %1, X: %2, Y: %3")
                .arg(
                    QString::number(distance), QString::number(angleX),
                    QString::number(angleY)
                );
            break;
        }
        return "Error";
    }

    PMat4 matrix() const {
        const auto rotation =
            PMat4::rotationY(radianY) * PMat4::rotationX(radianX);
        switch (type) {
        case Free: {
            const auto direction = rotation * PVec4{0.f, 0.f, 1.f, 0.f};
            return PMat4::lookTo(position, direction);
        }
        case Orbit: {
            const auto orbitPosition =
                rotation * PVec4{0.f, 0.f, -distance, 1.f};
            return PMat4::lookAt(orbitPosition, target);
        }
        }
        throw new std::logic_error("Unknown camera type");
    }
};

#endif // SCENE_INFO_H
