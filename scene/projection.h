#ifndef PROJECTION_H
#define PROJECTION_H

#include <QString>

#include "../pmath.h"

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

#endif // PROJECTION_H
