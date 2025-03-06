#include <QtOpenGL>

struct PIdx4 {
    uint offset;
    PIdx4(uint r, uint c) : offset(r << 2 | c) {}
};

struct PMat4 {
private:
    template <uint stride> class PLineRef {
        friend PMat4;

    private:
        inline PLineRef(GLfloat *p) : offsetValues(p) {}

        GLfloat *const offsetValues;

    public:
        inline GLfloat &operator[](uint index) {
            return offsetValues[index * stride];
        }
    };

public:
    inline PMat4() { memset(values, 0, sizeof(values)); }

    inline PLineRef<1> row(uint index) { return (values + index * 4); }
    inline PLineRef<4> col(uint index) { return (values + index); }

    inline GLfloat &operator[](PIdx4 index) { return values[index.offset]; }
    inline GLfloat &at(uint row, uint col) {
        return operator[](PIdx4(row, col));
    }

    GLfloat values[4 * 4];
};

struct PVec4 {
public:
    inline PVec4() { memset(values, 0, sizeof(values)); }

    inline GLfloat &operator[](uint index) { return values[index]; }

    GLfloat values[4];
};

static PMat4 operator*(PMat4 &left, PMat4 &right) {
    PMat4 res;
#pragma unroll
    for (int i = 0; i < 4; i++) {
        auto lRow = left.row(i);
#pragma unroll
        for (int j = 0; j < 4; j++) {
            auto rCol = right.col(j);
#pragma unroll
            for (int k = 0; k < 4; k++) {
                res.at(i, j) += lRow[k] * rCol[k];
            }
        }
    }
}
