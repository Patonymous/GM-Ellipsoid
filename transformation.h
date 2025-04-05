#ifndef TRANSFORMATION_H
#define TRANSFORMATION_H

#include "scene/model.h"

class ITransformation {
public:
    ITransformation()          = default;
    virtual ~ITransformation() = default;

    virtual Model transform(const Model &model) const = 0;
};

class Translation : public ITransformation {
public:
    Translation(PVec4 vector) : m_vector(vector) {};
    ~Translation() override = default;

    Model transform(const Model &model) const override {
        return Model(model.scaling, model.position + m_vector, model.rotation);
    }

private:
    PVec4 m_vector;
};

class Rotation : public ITransformation {
public:
    Rotation(PQuat rotation, PVec4 center)
        : m_rotation(rotation), m_center(center) {};
    ~Rotation() override = default;

    Model transform(const Model &model) const override {
        return Model(
            model.scaling,
            m_rotation.rotate(model.position - m_center) + m_center,
            m_rotation * model.rotation
        );
    }

private:
    PQuat m_rotation;
    PVec4 m_center;
};

class Scaling : public ITransformation {
public:
    Scaling(PVec4 scaling, PVec4 center)
        : m_scaling(scaling), m_center(center) {};
    ~Scaling() override = default;

    Model transform(const Model &model) const override {
        return Model(
            m_scaling.scale(model.scaling),
            m_scaling.scale(model.position - m_center) + m_center,
            model.rotation
        );
    }

private:
    PVec4 m_scaling;
    PVec4 m_center;
};

#endif // TRANSFORMATION_H
