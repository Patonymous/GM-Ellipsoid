#ifndef PMATH_INCLUDED
#define PMATH_INCLUDED

#include <QDebug>
#include <QString>

#define CONST_FUNC constexpr

constexpr float PI      = 3.14159f;
constexpr float EPSILON = 1E-6f;

inline CONST_FUNC float pAbs(float v) { return v < 0 ? -v : v; }

inline CONST_FUNC bool pEqual(float a, float b, float eps = EPSILON) {
    return pAbs(a - b) <= eps;
}

// adapted from https://en.cppreference.com/w/cpp/numeric/bit_cast
template <class To, class From>
std::enable_if_t<sizeof(To) == sizeof(From), To> pBitCast(const From &src) {
    To dst;
    std::memcpy(&dst, &src, sizeof(To));
    return dst;
}

// adapted from https://en.wikipedia.org/wiki/Fast_inverse_square_root
inline float pInvSqrt(float v) {
    auto f = pBitCast<float>(0x5F1FFFF9 - (pBitCast<long>(v) >> 1));
    return f * 0.703952253f * (2.38924456f - (v * f * f));
}

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

    inline CONST_FUNC float &operator[](uint index) {
        return this->*coords[index];
    }
    inline CONST_FUNC const float &operator[](uint index) const {
        return this->*coords[index];
        // switch (index) {
        // case 0:
        //     return x;
        // case 1:
        //     return y;
        // case 2:
        //     return z;
        // }
        // return w;
    }

    inline CONST_FUNC float dot(const PVec4 &right) const {
        return x * right.x + y * right.y + z * right.z;
    }
    inline CONST_FUNC float length() const { return dot(*this); };

    inline CONST_FUNC PVec4 operator+(const PVec4 &right) const {
        return {x + right.x, y + right.y, z + right.z};
    }
    inline CONST_FUNC PVec4 operator-(const PVec4 &right) const {
        return {x - right.x, y - right.y, z - right.z};
    }

    inline CONST_FUNC PVec4 operator*(float scale) const {
        return {x * scale, y * scale, z * scale};
    }

    inline PVec4 normalize() const {
        auto factor = pInvSqrt(length());
        return (*this) * factor;
    }

    inline CONST_FUNC PVec4 cross(const PVec4 &right) const {
        return {
            y * right.z - z * right.y, z * right.x - x * right.z,
            x * right.y - y * right.x
        };
    }

private:
    typedef float PVec4::*const    MemberPointer[4];
    static constexpr MemberPointer coords = {
        &PVec4::x, &PVec4::y, &PVec4::z, &PVec4::w
    };
};

inline PVec4 operator*(float scale, PVec4 &vec) { return vec * scale; }

struct PIdx4 {
    uint       offset;
    CONST_FUNC PIdx4(uint r, uint c) : offset((r << 2) | c) {}
};

struct PMat4 {
private:
    template <uint stride> class PLineRef {
        friend PMat4;

    private:
        CONST_FUNC inline PLineRef(float *p) : offsetValues(p) {}

        float *offsetValues;

    public:
        CONST_FUNC inline float &operator[](uint index) {
            return offsetValues[index * stride];
        }
    };
    template <uint stride> class PConstLineRef {
        friend PMat4;

    private:
        CONST_FUNC inline PConstLineRef(const float *p) : offsetValues(p) {}

        const float *offsetValues;

    public:
        CONST_FUNC inline const float &operator[](uint index) const {
            return offsetValues[index * stride];
        }
    };

public:
    // Common
    inline CONST_FUNC static PMat4
    diagonal(float x, float y, float z, float w = 1) {
        PMat4 res;
        res[{0, 0}] = x;
        res[{1, 1}] = y;
        res[{2, 2}] = z;
        res[{3, 3}] = w;
        return res;
    }
    inline CONST_FUNC static PMat4 identity() { return diagonal(1, 1, 1, 1); }
    // Transformation
    inline CONST_FUNC static PMat4 scaling(float x, float y, float z) {
        return diagonal(x, y, z);
    }
    inline CONST_FUNC PMat4 translation(float x, float y, float z) {
        PMat4 res = identity();

        res[{0, 3}] = x;
        res[{1, 3}] = y;
        res[{2, 3}] = z;

        return res;
    }
    inline static PMat4 rotationX(float radians) {
        PMat4 res = identity();

        auto cos = cosf(radians);
        auto sin = sinf(radians);

        res[{1, 1}] = cos;
        res[{1, 2}] = -sin;
        res[{2, 1}] = sin;
        res[{2, 2}] = cos;

        return res;
    }
    inline static PMat4 rotationY(float radians) {
        PMat4 res = identity();

        auto cos = cosf(radians);
        auto sin = sinf(radians);

        res[{0, 0}] = cos;
        res[{0, 2}] = sin;
        res[{2, 0}] = -sin;
        res[{2, 2}] = cos;

        return res;
    }
    inline static PMat4 rotationZ(float radians) {
        PMat4 res = identity();

        auto cos = cosf(radians);
        auto sin = sinf(radians);

        res[{0, 0}] = cos;
        res[{0, 1}] = -sin;
        res[{1, 0}] = sin;
        res[{1, 1}] = cos;

        return res;
    }
    // Camera
    inline static PMat4 lookAt(PVec4 position, PVec4 target, PVec4 up) {
        const auto zAxis = (position - target).normalize();
        const auto xAxis = zAxis.cross(up).normalize();
        const auto yAxis = xAxis.cross(zAxis);

        float res[4 * 4] = {xAxis.x, yAxis.x, zAxis.x, position.x,
                            xAxis.y, yAxis.y, zAxis.y, position.y,
                            xAxis.z, yAxis.z, zAxis.z, position.z,
                            0,       0,       0,       1};
        return PMat4(res).inverse();
    }
    // Projection
    inline static PMat4 orthographic(
        float width, float height, float near, float far,
        PVec4 direction = {0, 0, 1}
    ) {
        auto deltaX = direction.x / direction.z;
        auto deltaY = direction.y / direction.z;

        auto depth = far - near;
        auto weird = -(far + near) / depth;

        float res[4 * 4] = {2 / width, 0,          -deltaX,    0, //
                            0,         2 / height, -deltaY,    0, //
                            0,         0,          -2 / depth, weird,
                            0,         0,          0,          1};
        return res;
    }
    inline static PMat4 perspective(
        float heightToWidthRatio, float fieldOfView, float near, float far
    ) {
        const auto ratio  = heightToWidthRatio;
        const auto fov    = 1.f / tanf(fieldOfView / 2);
        const auto depth  = far - near;
        const auto weird1 = -(far + near) / depth;
        const auto weird2 = -2 * far * near / depth;

        float res[4 * 4] = {fov, 0,           0,      0,      //
                            0,   fov / ratio, 0,      0,      //
                            0,   0,           weird1, weird2, //
                            0,   0,           -1,     0};
        return res;
    }

    float values[4 * 4] = {};

    inline CONST_FUNC PMat4() {}
    inline CONST_FUNC PMat4(const float *valuesToCopy) {
        for (uint i = 0; i < sizeof(values) / sizeof(*values); i++)
            values[i] = valuesToCopy[i];
    }

    inline operator QString() const {
        QString res('[');
        for (uint r = 0; r < 4; r++) {
            auto row = cRow(r);
            if (r != 0)
                res.append(';');
            res.append(QString("%1,%2,%3,%4")
                           .arg(
                               QString::number(row[0], 'f', 2),
                               QString::number(row[1], 'f', 2),
                               QString::number(row[2], 'f', 2),
                               QString::number(row[3], 'f', 2)
                           ));
        }
        return res.append(']');
    }

    inline CONST_FUNC PLineRef<1> row(uint index) {
        return {values + index * 4};
    }
    inline CONST_FUNC PConstLineRef<1> cRow(uint index) const {
        return {values + index * 4};
    }
    inline CONST_FUNC PLineRef<4> col(uint index) { return {values + index}; }
    inline CONST_FUNC PConstLineRef<4> cCol(uint index) const {
        return {values + index};
    }

    inline CONST_FUNC float &operator[](PIdx4 index) {
        return values[index.offset];
    }
    inline CONST_FUNC const float &operator[](PIdx4 index) const {
        return values[index.offset];
    }

    inline CONST_FUNC PMat4 operator*(const PMat4 &right) const {
        PMat4 res;
        for (uint i = 0; i < 4; i++) {
            auto lRow = cRow(i);
            for (uint j = 0; j < 4; j++) {
                auto rCol = right.cCol(j);
                for (uint k = 0; k < 4; k++) {
                    res[{i, j}] += lRow[k] * rCol[k];
                }
            }
        }
        return res;
    }
    inline CONST_FUNC PVec4 operator*(const PVec4 &right) const {
        PVec4 res;
        for (uint i = 0; i < 4; i++) {
            auto lRow = cRow(i);
            for (uint j = 0; j < 4; j++)
                res[i] += lRow[j] * right[j];
        }
        if (res[3] != 1) {
            for (uint i = 0; i < 3; i++)
                res[i] /= res[3];
            res[3] = 1;
        }
        return res;
    }

    inline CONST_FUNC PMat4 transpose() const {
        PMat4 res;
        for (uint r = 0; r < 4; r++)
            for (uint c = 0; c < 4; c++)
                res[{r, c}] = (*this)[{c, r}];
        return res;
    }

    inline CONST_FUNC PMat4 inverse() const {
        auto src = *this;
        auto res = identity();

        // Gaussian elimination with partial pivoting
        for (uint c = 0; c < 4; c++) {
            // Select pivot
            uint  bestR = c;
            float best  = pAbs(src[{bestR, c}]);
            for (uint r = c + 1; r < 4; r++) {
                if (pAbs(src[{r, c}]) <= best)
                    continue;
                bestR = r;
                best  = pAbs(src[{r, c}]);
            }
            // Swap rows
            if (bestR != c) {
                for (uint c2 = 0; c2 < 4; c2++) {
                    qSwap(src[{c, c2}], src[{bestR, c2}]);
                    qSwap(res[{c, c2}], res[{bestR, c2}]);
                }
            }
            // Subtract current row (scaled) from those below
            for (uint r = c + 1; r < 4; r++) {
                float factor = src[{r, c}] / src[{c, c}];
                for (uint c2 = c; c2 < 4; c2++)
                    src[{r, c2}] -= factor * src[{c, c2}];
                for (uint c2 = 0; c2 < 4; c2++)
                    res[{r, c2}] -= factor * res[{c, c2}];
            }
        }

        // Zero values above diagonal starting from rightmost column
        for (uint c = 4; c > 0;) {
            --c;
            // Subtract current row (scaled) from those above
            for (uint r = 0; r < c; r++) {
                float factor = src[{r, c}] / src[{c, c}];
                for (uint c2 = 0; c2 < 4; c2++)
                    res[{r, c2}] -= factor * res[{c, c2}];
            }
        }

        // Multiply rows so that source matrix becomes an identity
        for (uint r = 0; r < 4; r++) {
            float factor = 1.f / src[{r, r}];
            for (uint c = 0; c < 4; c++)
                res[{r, c}] *= factor;
        }

        return res;
    }
};

#endif // PMATH_INCLUDED
