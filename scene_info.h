#ifndef SCENE_INFO_H
#define SCENE_INFO_H

#include "pmath.h"

struct SceneInfo {
    enum Projection {
        Orthographic,
        Perspective,
    } projectionType;
    enum Camera {
        Free,
        Orbit,
    } cameraType;
    float width, height, near, far;
    float cameraDistance, cameraRadianX, cameraRadianY;
    PVec4 cameraPosition, cameraTarget;

    SceneInfo(
        Projection p, Camera c,             //
        float w, float h, float n, float f, //
        float cd, float cx, float cy,       //
        const PVec4 &cp, const PVec4 &ct
    )
        : projectionType(p), cameraType(c),                         //
          width(w), height(h), near(n), far(f),                     //
          cameraDistance(cd), cameraRadianX(cx), cameraRadianY(cy), //
          cameraPosition(cp), cameraTarget(ct) {}

    PMat4 projectionMatrix() const {
        switch (projectionType) {
        case Orthographic:
            return PMat4::orthographic(width, height, near, far);

        case Perspective:
            return PMat4::perspective(height / width, PI_F / 2, near, far);
        }
        throw new std::exception("Unknown projection type");
    }
    PMat4 viewMatrix() const {
        switch (cameraType) {
        case Free: {
            const auto direction = cameraMatrix() * PVec4{0.f, 0.f, 1.f, 0.f};
            return PMat4::lookTo(cameraPosition, direction);
        }
        case Orbit: {
            const auto orbitPosition =
                cameraMatrix() * PVec4{0.f, 0.f, -cameraDistance, 1.f};
            return PMat4::lookAt(orbitPosition, cameraTarget);
        }
        }
        throw new std::exception("Unknown camera type");
    }

    // TODO: Lights

private:
    PMat4 cameraMatrix() const {
        return PMat4::rotationY(cameraRadianY)
             * PMat4::rotationX(cameraRadianX);
    }
};

#endif // SCENE_INFO_H
