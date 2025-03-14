#include "renderable.h"

void OpenGLArea::render(const IRenderable &renderable) {
    m_toRender.append(&renderable);
    ensureUpdatePending();
}

void OpenGLArea::initializeGL() {
    //
}

void OpenGLArea::paintGL() {
    auto glCtx = context();
    for (auto &renderable : m_toRender)
        renderable->render(glCtx);

    m_updatePending = false;
}

void OpenGLArea::ensureUpdatePending() {
    if (!m_updatePending)
        return;

    m_updatePending = true;
    update();
}
