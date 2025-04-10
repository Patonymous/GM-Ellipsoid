#include <QtConcurrent>

#include "ellipsoid.h"
#include "renderer.h"

constexpr qint64 FRAME_INTERVAL_MS = 10;

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
        : Position2D{startingX, startingY}, width{w}, height{h}, step{s} {}

public:
    typedef int                       difference_type;
    typedef Position2D                value_type;
    typedef void                      pointer;
    typedef void                      reference;
    typedef std::forward_iterator_tag iterator_category;

    inline static ScreenIterator2D begin(uint w, uint h, uint s) {
        return ScreenIterator2D{w, h, s};
    }

    inline static ScreenIterator2D end(uint w, uint h, uint s) {
        return ScreenIterator2D{w, h, s, PAST_THE_END, PAST_THE_END};
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
    : QObject(nullptr), m_timer{}, m_pvme{}, m_pvInverse{},
      m_ellipsoid{ellipsoid} {
    m_timer.start();
}
Renderer::~Renderer() { m_timer.invalidate(); }

void Renderer::setupConnection() {
    QObject::connect(
        m_ellipsoid, &Ellipsoid::renderRequested, this,
        &Renderer::renderEllipsoid, Qt::QueuedConnection
    );
}

void Renderer::renderEllipsoid(Params params) {
    DPRINT("Starting rendering...");

    m_equation = PMat4::diagonal(
        1.f / (params.stretchX * params.stretchX),
        1.f / (params.stretchY * params.stretchY),
        1.f / (params.stretchZ * params.stretchZ), -1
    );
    m_camera = PMat4::rotationY(params.cameraAngleY)
             * PMat4::rotationX(params.cameraAngleX)
             * PVec4(0, 0, params.cameraDistance);
    auto model =
        PMat4::translation(params.positionX, params.positionY, params.positionZ)
        * PMat4::scaling(params.scale, params.scale, params.scale);
    auto view = PMat4::lookAt(m_camera, {}, {0, 1, 0});
    // auto projection = PMat4::orthographic(20.f, 20.f, 0.1f, 19.9f);
    auto projection =
        PMat4::perspective(params.height / params.width, PI_F / 2, 0.1f, 19.9f);

    auto pv     = projection * view;
    m_pvInverse = pv.inverse();

    auto pvmInverse = (pv * model).inverse();
    m_pvme          = pvmInverse.transpose() * m_equation * pvmInverse;

    const auto pixels = m_ellipsoid->m_pixelData.data();

    // TODO: Add cancellation
    QtConcurrent::blockingMap(
        ScreenIterator2D::begin(
            params.width, params.height, params.pixelGranularity
        ),
        ScreenIterator2D::end(
            params.width, params.height, params.pixelGranularity
        ),
        [this, pixels, &params](Position2D pos) {
            const auto &[x, y] = pos;

            const auto
                &[w, h, sub, r, g, b, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10,
                  a, d, s, sf] = params;

            const auto intensity = lightIntensityAtCastRay(
                (x * 2.f + 1) / w - 1, (y * 2.f + 1) / h - 1, a, d, s, sf
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

    DPRINT("Rendering completed.");
    emit renderCompleted();
}

float Renderer::lightIntensityAtCastRay(
    float x, float y, float ambient, float diffuse, float specular,
    float specularFocus
) // both in range <-1,+1>
{
    const float r = 1;

    const auto a = m_pvme[{2, 2}];
    const auto b = (m_pvme[{0, 2}] + m_pvme[{2, 0}]) * x
                 + (m_pvme[{1, 2}] + m_pvme[{2, 1}]) * y
                 + (m_pvme[{3, 2}] + m_pvme[{2, 3}]) * r;
    const auto c = m_pvme[{0, 0}] * x * x + m_pvme[{1, 1}] * y * y
                 + (m_pvme[{0, 1}] + m_pvme[{1, 0}]) * x * y
                 + (m_pvme[{0, 3}] + m_pvme[{3, 0}]) * x * r
                 + (m_pvme[{1, 3}] + m_pvme[{3, 1}]) * y * r
                 + m_pvme[{3, 3}] * r * r;

    const auto delta = b * b - 4 * a * c;

    if (delta < 0)
        return 0;

    const auto root = qSqrt(delta);
    const auto z    = qMin((-b - root) / (2 * a), (-b + root) / (2 * a));

    const auto worldPosition = m_pvInverse * PVec4{x, y, z};

    const auto worldNormal =
        PVec4{
            2 * worldPosition.x * m_equation[{0, 0}],
            2 * worldPosition.y * m_equation[{1, 1}],
            2 * worldPosition.z * m_equation[{2, 2}], 0.f
        }
            .normalize();
    const auto toCamera  = (m_camera - worldPosition).normalize();
    const auto toLight   = toCamera; // we assume they're in the same place
    const auto reflected = worldNormal.reflect(-toLight);

    const auto ambientIntensity = ambient;
    const auto diffuseIntensity = diffuse * qMax(worldNormal.dot(toLight), 0.f);
    const auto specularIntensity =
        specular * powf(qMax(reflected.dot(toCamera), 0.f), specularFocus);

    return qBound(
        0.f, ambientIntensity + diffuseIntensity + specularIntensity, 1.f
    );
}
