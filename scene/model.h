#ifndef MODEL_H
#define MODEL_H

#include <QKeyEvent>
#include <QString>

#include "../helpers.h"
#include "../pmath.h"

struct Model : public QObject {
    Q_OBJECT

public:
    PVec4 scaling;
    PVec4 position;
    PQuat rotation;

    Model()
        : scaling({1.f, 1.f, 1.f, 0.f}), position({0.f, 0.f, 0.f, 1.f}),
          rotation({1.f, 0.f, 0.f, 0.f}) {}
    Model(PVec4 s, PVec4 p, PQuat r) : scaling(s), position(p), rotation(r) {}

    operator QString() const {
        return QString("S:%1, P:%2, R:%3")
            .arg((QString)scaling, (QString)position, (QString)rotation);
    }

    PMat4 matrix() const {
        return PMat4::translation(position) * PMat4::rotation(rotation)
             * PMat4::scaling(scaling);
    }
};

#endif // MODEL_H
