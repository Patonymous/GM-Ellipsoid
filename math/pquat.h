#ifndef PQUAT_H
#define PQUAT_H

#include <QDebug>
#include <QString>

#include "../helpers.h"
#include "pvec4.h"

struct PQuat {
    float r;
    float i;
    float j;
    float k;

    inline static PQuat Rotation(float radian, PVec4 axis) {
        PQuat res;
        float c = cosf(radian);
        float s = sinf(radian);
        return {c, s * axis.x, s * axis.y, s * axis.z};
    }

    inline CONST_FUNC PQuat() : r{0}, i{0}, j{0}, k{0} {}
    inline CONST_FUNC PQuat(float r, float i, float j, float k)
        : r{r}, i{i}, j{j}, k{k} {}

    inline operator QString() const {
        return QString("{%1,[%2,%3,%4]}")
            .arg(
                QString::number(r, 'f', 2), QString::number(i, 'f', 2),
                QString::number(j, 'f', 2), QString::number(k, 'f', 2)
            );
    }

    inline CONST_FUNC float &operator[](uint index) {
        return this->*coords[index];
    }
    inline CONST_FUNC const float &operator[](uint index) const {
        return this->*coords[index];
    }

    inline CONST_FUNC float magnitude() const {
        return r * r + i * i + j * j + k * k;
    }

    inline PQuat normalize() const { return *this * pInvSqrt(magnitude()); }

    inline CONST_FUNC PQuat conjugate() const { return {r, -i, -j, -k}; }

    inline CONST_FUNC PQuat operator*(float right) const {
        return {r * right, i * right, j * right, k * right};
    }

    inline PQuat operator*(const PQuat &right) const {
        return PQuat{
            r * right.r - i * right.i - j * right.j - k * right.k,
            r * right.i + i * right.r + j * right.k - k * right.j,
            r * right.j - i * right.k + j * right.r + k * right.i,
            r * right.k + i * right.j - j * right.i + k * right.r,
        }
            .normalize();
    }
    inline PQuat &operator*=(const PQuat &right) {
        return *this = *this * right;
    }

private:
    typedef float PQuat::*const    MemberPointer[4];
    static constexpr MemberPointer coords = {
        &PQuat::r, &PQuat::i, &PQuat::j, &PQuat::k
    };
};

inline CONST_FUNC PQuat operator*(float scale, PQuat &qua) {
    return qua * scale;
}

#endif // PQUAT_H
