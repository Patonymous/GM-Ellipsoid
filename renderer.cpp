#include <QtConcurrent>

#include "ellipsoid.h"

constexpr qint64 FRAME_INTERVAL_MS = 500;

struct Position2D {
    uint x = 0, y = 0;

    operator QString() const {
        return QString("Pixel:%1,%2")
            .arg(QString::number(x), QString::number(y));
    }
};

class ScreenIterator2D : Position2D {
    static constexpr uint PAST_THE_END = (uint)-1;

    uint width, height, step;

    ScreenIterator2D(
        uint w, uint h, uint s, uint startingX = 0, uint startingY = 0
    )
        : Position2D{ startingX, startingY }, width{ w }, height{ h },
          step{ s } {}

public:
    typedef int                       difference_type;
    typedef Position2D                value_type;
    typedef void                      pointer;
    typedef void                      reference;
    typedef std::forward_iterator_tag iterator_category;

    inline static ScreenIterator2D begin(uint w, uint h, uint s) {
        return ScreenIterator2D{ w, h, s };
    }

    inline static ScreenIterator2D end(uint w, uint h, uint s) {
        return ScreenIterator2D{ w, h, s, PAST_THE_END, PAST_THE_END };
    }

    operator QString() const {
        return QString("Iterator:%1/%3,%2/%4+%5")
            .arg(
                QString::number(x), QString::number(y), QString::number(width),
                QString::number(height), QString::number(step)
            );
    }

    inline bool operator==(const ScreenIterator2D &other) const {
        return x == other.x && y == other.y;
    }
    inline bool operator!=(const ScreenIterator2D &other) const {
        return !(*this == other);
    }

    inline Position2D operator*() { return *this; }

    inline ScreenIterator2D &operator++() {
        x += step;
        if (x < width)
            return *this;
        x  = 0;
        y += step;
        if (y < height)
            return *this;
        x = PAST_THE_END;
        y = PAST_THE_END;
        return *this;
    }
    inline ScreenIterator2D operator++(int) {
        const auto copy = *this;
        ++(*this);
        return copy;
    }
};

Renderer::Renderer(Ellipsoid *ellipsoid)
    : QObject(nullptr), m_timer{}, m_ellipsoid{ ellipsoid } {
    m_timer.start();
}
Renderer::~Renderer() { m_timer.invalidate(); }

void Renderer::setupConnection() {
    QObject::connect(
        m_ellipsoid, &Ellipsoid::requestRender, this,
        &Renderer::renderEllipsoid, Qt::QueuedConnection
    );
}

void Renderer::renderEllipsoid(Params params) {
    qDebug() << "Starting rendering...";

    const auto pixels = m_ellipsoid->m_pixelData.data();

    QtConcurrent::blockingMap(
        ScreenIterator2D::begin(
            params.width, params.height, params.pixelGranularity
        ),
        ScreenIterator2D::end(
            params.width, params.height, params.pixelGranularity
        ),
        [this, pixels, &params](Position2D pos) {
            const auto &[x, y] = pos;

            const auto &[w, h, sub, r, g, b, _1, _2, _3, _4, _5] = params;

            const auto intensity = lightIntensityAtCastRay(
                (x * 2.f + 1) / w - 1, (y * 2.f + 1) / h - 1
            );

            const auto xEnd = qMin(x + sub, w);
            const auto yEnd = qMin(y + sub, h);
            for (uint i = y; i < yEnd; i++) {
                for (uint j = x; j < xEnd; j++) {
                    pixels[(i * w + j) * 4 + 0] = r * intensity;
                    pixels[(i * w + j) * 4 + 1] = g * intensity;
                    pixels[(i * w + j) * 4 + 2] = b * intensity;
                    pixels[(i * w + j) * 4 + 3] = 255;
                }
            }
        }
    );

    const auto sinceLastFrameMs = m_timer.elapsed();
    if (sinceLastFrameMs < FRAME_INTERVAL_MS)
        QThread::msleep(FRAME_INTERVAL_MS - sinceLastFrameMs);
    m_timer.start();

    qDebug() << "Rendering completed.";
    emit renderCompleted(params);
}

float Renderer::lightIntensityAtCastRay(
    float x, float y
) // both in range <-1,+1>
{
    float intensity = (x * x + y * y) < 0.25 ? 1 : 0;
    return qBound(0.f, intensity, 1.f);
}
