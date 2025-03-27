#ifndef MODEL_H
#define MODEL_H

#include <QKeyEvent>
#include <QString>

#include "../helpers.h"
#include "../pmath.h"

constexpr float OBJECT_MOVEMENT_SPEED = 0.05f;
constexpr float OBJECT_ROTATION_SPEED = PI_F / 36.f;

struct Model {
    bool lockScale, lockPosition, lockRotation;

    PVec4 scale, position;
    PQuat rotation;

    Model(bool ls, bool lp, bool lr, PVec4 s, PVec4 p, PQuat r)
        : lockScale(ls), lockPosition(lp), lockRotation(lr), scale(s),
          position(p), rotation(r) {}

    operator QString() const {
        return QString("S:%1, P:%2, R:%3")
            .arg((QString)scale, (QString)position, (QString)rotation);
    }

    PMat4 matrix() const {
        return PMat4::translation(position) * PMat4::rotation(rotation)
             * PMat4::scaling(scale);
    }

    bool updateByKeyboardControls(QKeyEvent *event) {
        switch (event->key()) {
        case Qt::Key_R:
            if (!lockPosition)
                position.z -= OBJECT_MOVEMENT_SPEED;
            break;
        case Qt::Key_Y:
            if (!lockPosition)
                position.z += OBJECT_MOVEMENT_SPEED;
            break;
        case Qt::Key_T:
            if (!lockPosition)
                position.y += OBJECT_MOVEMENT_SPEED;
            break;
        case Qt::Key_G:
            if (!lockPosition)
                position.y -= OBJECT_MOVEMENT_SPEED;
            break;
        case Qt::Key_F:
            if (!lockPosition)
                position.x -= OBJECT_MOVEMENT_SPEED;
            break;
        case Qt::Key_H:
            if (!lockPosition)
                position.x += OBJECT_MOVEMENT_SPEED;
            break;

        case Qt::Key_U:
            if (!lockRotation)
                rotation *=
                    PQuat::Rotation(-OBJECT_ROTATION_SPEED, {0.f, 0.f, 1.f});
            break;
        case Qt::Key_O:
            if (!lockRotation)
                rotation *=
                    PQuat::Rotation(+OBJECT_ROTATION_SPEED, {0.f, 0.f, 1.f});
            break;
        case Qt::Key_I:
            if (!lockRotation)
                rotation *=
                    PQuat::Rotation(+OBJECT_ROTATION_SPEED, {1.f, 0.f, 0.f});
            break;
        case Qt::Key_K:
            if (!lockRotation)
                rotation *=
                    PQuat::Rotation(-OBJECT_ROTATION_SPEED, {1.f, 0.f, 0.f});
            break;
        case Qt::Key_J:
            if (!lockRotation)
                rotation *=
                    PQuat::Rotation(-OBJECT_ROTATION_SPEED, {0.f, 1.f, 0.f});
            break;
        case Qt::Key_L:
            if (!lockRotation)
                rotation *=
                    PQuat::Rotation(+OBJECT_ROTATION_SPEED, {0.f, 1.f, 0.f});
            break;

        default:
            return false;
        }
        return true;
    }
};

#endif // MODEL_H
