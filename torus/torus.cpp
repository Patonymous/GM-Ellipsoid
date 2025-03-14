#include <QOpenGLDebugLogger>

#include "../helpers.h"
#include "torus.h"

Torus::Torus() : m_tSamples(4), m_sSamples(4) {}
Torus::~Torus() {}

QString Torus::debugId() const {
    return QString("Torus:%1").arg(reinterpret_cast<qptrdiff>(this));
}

void Torus::initializeGL(QOpenGLContext *context) {
    //
}

void Torus::paintGL(QOpenGLContext *context) {
    //
}

void Torus::setTSamples(int value) {
    m_tSamples = value;
    emit needRepaint();
}

int Torus::tSamples() const { return m_tSamples; }

void Torus::setSSamples(int value) {
    m_sSamples = value;
    emit needRepaint();
}

int Torus::sSamples() const { return m_sSamples; }
