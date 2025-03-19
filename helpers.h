#ifndef HELPERS_INCLUDED
#define HELPERS_INCLUDED

#include <QTime>

#define CONST_FUNC constexpr

// clang-format off
// #define DPRINT(x) qDebug() << QTime::currentTime().msecsSinceStartOfDay() << x;
#define DPRINT(x)
// clang-format on

constexpr float  PI_F      = 3.14159265f;
constexpr double PI_D      = 3.141592653689793;
constexpr float  EPSILON_F = 1E-6f;
constexpr double EPSILON_D = 1E-12;

inline CONST_FUNC float pAbsF(float v) { return v < 0 ? -v : v; }

inline CONST_FUNC double pAbsD(double v) { return v < 0 ? -v : v; }

inline CONST_FUNC bool pEqualF(float a, float b, float eps = EPSILON_F) {
    return pAbsF(a - b) <= eps;
}

inline CONST_FUNC bool pEqualD(double a, double b, double eps = EPSILON_D) {
    return pAbsF(a - b) <= eps;
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
    auto f = pBitCast<float>(0x5F1FFFF9 - (pBitCast<int32_t>(v) >> 1));
    return f * 0.703952253f * (2.38924456f - (v * f * f));
}
#endif // HELPERS_INCLUDED
