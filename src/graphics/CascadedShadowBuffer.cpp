#include "graphics/CascadedShadowBuffer.h"

CascadedShadowBuffer::CascadedShadowBuffer(float near, float far) : Framebuffer(), near(near), far(far) {
}

CascadedShadowBuffer::~CascadedShadowBuffer() {
}

void CascadedShadowBuffer::create() {
    glGenTextures(1, &m_TextureBuffer);
    glBindTexture(GL_TEXTURE_2D_ARRAY, m_TextureBuffer);
    glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_DEPTH_COMPONENT32F, m_Width, m_Height, int(cascade_levels.size()) + 1, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);

    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

    constexpr float bordercolor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_BORDER_COLOR, bordercolor);

    glBindFramebuffer(GL_FRAMEBUFFER, m_Framebuffer);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_TextureBuffer, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    int status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "ERROR::CASCADED_FRAMEBUFFER::Cascaded framebuffer is not complete!" << std::endl;
    }
    FRAMEBUFFER_END();
}

void CascadedShadowBuffer::resize(unsigned int width, unsigned int height) {
}