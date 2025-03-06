#include <QtConcurrent>

#include "ellipsoid.h"

struct Position2D {
    uint x, y;
};

// TODO: Implement all required parts of iterator interface
class ScreenIterator2D {
    uint x, y, width, height;

public:
    ScreenIterator2D(uint w, uint h) : width{ w }, height{ h } {}

    inline bool operator==(const ScreenIterator2D &other) const {
        return x == other.x && y == other.y;
    }
    inline bool operator!=(const ScreenIterator2D &other) const {
        return !((*this) == other);
    }

    inline Position2D operator*() { return { x, y }; }

    inline ScreenIterator2D &operator++() {
        if (++x != width)
            return *this;
        x = 0;
        if (++y != height)
            return *this;
        y = 0;
        return *this;
    }
    inline ScreenIterator2D operator++(int) {
        const auto copy = *this;
        ++(*this);
        return copy;
    }

    // return { (x * 2.f + 1) / width - 1, (y * 2.f + 1) / height - 1 }
};

Renderer::Renderer(Ellipsoid *ellipsoid)
    : QObject(ellipsoid), m_ellipsoid{ ellipsoid } {
    QObject::connect(
        m_ellipsoid, &Ellipsoid::requestRender, this,
        &Renderer::renderEllipsoid, Qt::QueuedConnection
    );
}
Renderer::~Renderer() {}

void Renderer::renderEllipsoid(Params params) {
    const auto pixels = m_ellipsoid->m_pixelData.data();

    const auto w         = params.width;
    const auto h         = params.height;
    const auto subLength = params.pixelGranularity;

    const auto screen = ScreenIterator2D(params.width, params.height);
    QtConcurrent::blockingMap(screen, screen, [](Position2D pos) { return; });

    for (int y = 0; y < h; y += subLength) {
        for (int x = 0; x < w; x += subLength) {
            const auto intensity =
                lightIntensityAtCastRay(x * 2.f / w - 1, y * 2.f / h - 1);

            for (int sy = 0; y + sy < h && sy < subLength; sy++) {
                for (int sx = 0; x + sx < w && sx < subLength; sx++) {
                    pixels[((y + sy) * h + x + sx) * 4 + 0] =
                        params.materialRed * intensity;
                    pixels[((y + sy) * h + x + sx) * 4 + 1] =
                        params.materialGreen * intensity;
                    pixels[((y + sy) * h + x + sx) * 4 + 2] =
                        params.materialBlue * intensity;
                    pixels[((y + sy) * h + x + sx) * 4 + 3] = 255;
                }
            }
        }
    }

    emit renderCompleted(params);
}

float Renderer::lightIntensityAtCastRay(float x, float y) {
    return 0;
} // both in range <-1,+1>
  // {
  //     float intensity = (x * x + y * y) < 0.25 ? 1 : 0;

//     if (intensity < 0)
//         intensity = 0;
//     if (intensity > 1)
//         intensity = 1;
//     return intensity;
// }
