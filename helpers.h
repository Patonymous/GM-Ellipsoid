#ifndef HELPERS_INCLUDED
#define HELPERS_INCLUDED

#define CONST_FUNC constexpr

#define DPRINT(x) qDebug() << QTime::currentTime().msecsSinceStartOfDay() << x;
// #define DPRINT(x)

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

#endif // HELPERS_INCLUDED
