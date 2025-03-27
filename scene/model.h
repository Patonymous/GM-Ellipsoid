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
            position.z -= OBJECT_MOVEMENT_SPEED;
            break;
        case Qt::Key_Y:
            position.z += OBJECT_MOVEMENT_SPEED;
            break;
        case Qt::Key_T:
            position.y += OBJECT_MOVEMENT_SPEED;
            break;
        case Qt::Key_G:
            position.y -= OBJECT_MOVEMENT_SPEED;
            break;
        case Qt::Key_F:
            position.x -= OBJECT_MOVEMENT_SPEED;
            break;
        case Qt::Key_H:
            position.x += OBJECT_MOVEMENT_SPEED;
            break;

        case Qt::Key_U:
            rotation *=
                PQuat::Rotation(-OBJECT_ROTATION_SPEED, {0.f, 0.f, 1.f});
            break;
        case Qt::Key_O:
            rotation *=
                PQuat::Rotation(+OBJECT_ROTATION_SPEED, {0.f, 0.f, 1.f});
            break;
        case Qt::Key_I:
            rotation *=
                PQuat::Rotation(+OBJECT_ROTATION_SPEED, {1.f, 0.f, 0.f});
            break;
        case Qt::Key_K:
            rotation *=
                PQuat::Rotation(-OBJECT_ROTATION_SPEED, {1.f, 0.f, 0.f});
            break;
        case Qt::Key_J:
            rotation *=
                PQuat::Rotation(-OBJECT_ROTATION_SPEED, {0.f, 1.f, 0.f});
            break;
        case Qt::Key_L:
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
