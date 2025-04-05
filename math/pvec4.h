#ifndef PVEC4_H
#define PVEC4_H

#include <QDebug>
#include <QString>

#include "../helpers.h"

struct PVec4 {
public:
    float x;
    float y;
    float z;
    float w;

    inline CONST_FUNC PVec4() : x{0}, y{0}, z{0}, w{0} {}
    inline CONST_FUNC PVec4(float x, float y, float z, float w = 1)
        : x{x}, y{y}, z{z}, w{w} {}

    inline operator QString() const {
        return QString("[%1,%2,%3,%4]")
            .arg(
                QString::number(x, 'f', 2), QString::number(y, 'f', 2),
                QString::number(z, 'f', 2), QString::number(w, 'f', 2)
            );
    }
    inline explicit operator QVector3D() const { return QVector3D(x, y, z); }

    inline CONST_FUNC float &operator[](uint index) {
        return this->*coords[index];
    }
    inline CONST_FUNC const float &operator[](uint index) const {
        return this->*coords[index];
    }

    inline CONST_FUNC float dot(const PVec4 &right) const {
        return x * right.x + y * right.y + z * right.z;
    }
    inline CONST_FUNC float magnitude() const { return dot(*this); };

    inline CONST_FUNC PVec4 operator-() const { return {-x, -y, -z}; }

    inline CONST_FUNC PVec4 operator+(const PVec4 &right) const {
        return {x + right.x, y + right.y, z + right.z};
    }
    inline CONST_FUNC PVec4 &operator+=(const PVec4 &right) {
        return *this = *this + right;
    }

    inline CONST_FUNC PVec4 operator-(const PVec4 &right) const {
        return {x - right.x, y - right.y, z - right.z};
    }
    inline CONST_FUNC PVec4 &operator-=(const PVec4 &right) {
        return *this = *this - right;
    }

    inline CONST_FUNC PVec4 operator*(float scale) const {
        return {x * scale, y * scale, z * scale};
    }

    inline CONST_FUNC PVec4 operator/(float scale) const {
        return {x / scale, y / scale, z / scale};
    }

    inline CONST_FUNC PVec4 max(float min) const {
        return {qMax(x, min), qMax(y, min), qMax(z, min)};
    }

    inline CONST_FUNC PVec4 min(float max) const {
        return {qMin(x, max), qMin(y, max), qMin(z, max)};
    }

    inline CONST_FUNC PVec4 clamp(float min, float max) {
        return {qBound(min, x, max), qBound(min, y, max), qBound(min, z, max)};
    }

    inline PVec4 normalize() const {
        auto factor = pInvSqrt(magnitude());
        return (*this) * factor;
    }

    /// treats 'this' as normal and reflects incoming versor!
    inline CONST_FUNC PVec4 reflect(const PVec4 &versor) const {
        return versor - *this * 2 * dot(versor);
    }

    inline CONST_FUNC PVec4 cross(const PVec4 &right) const {
        return {
            y * right.z - z * right.y, z * right.x - x * right.z,
            x * right.y - y * right.x
        };
    }

    inline CONST_FUNC PVec4 scale(const PVec4 &right) const {
        return {x * right.x, y * right.y, z * right.z, right.w};
    }

private:
    typedef float PVec4::*const    MemberPointer[4];
    static constexpr MemberPointer coords = {
        &PVec4::x, &PVec4::y, &PVec4::z, &PVec4::w
    };
};

inline CONST_FUNC PVec4 operator*(float scale, PVec4 &vec) {
    return vec * scale;
}

#endif // PVEC4_H
