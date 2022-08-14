#include "graphics/Framebuffer.h"

Framebuffer::Framebuffer() {
}

Framebuffer::~Framebuffer() {
    if (m_TextureBuffer != 0)
        glDeleteTextures(1, &m_TextureBuffer);
    if (m_RenderBuffer != 0)
        glDeleteRenderbuffers(1, &m_RenderBuffer);
    if (m_Framebuffer != 0)
        glDeleteFramebuffers(1, &m_Framebuffer);
}

void Framebuffer::add_spec(FramebufferSpec spec) { m_Specs.push_back(spec); }

void Framebuffer::create() {
    glBindFramebuffer(GL_FRAMEBUFFER, m_Framebuffer);
    for (const auto &spec : m_Specs) {
        switch (spec) {
            case FramebufferSpec::COLOR:
                // create a color attachment texture
                glGenTextures(1, &m_TextureBuffer);
                glBindTexture(GL_TEXTURE_2D, m_TextureBuffer);
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, m_Width, m_Height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_TextureBuffer, 0);
                break;
            case FramebufferSpec::DEPTH24STENCIL8:
                // create a renderbuffer object for depth and stencil attachment (we won't be sampling these)
                glGenRenderbuffers(1, &m_RenderBuffer);
                glBindRenderbuffer(GL_RENDERBUFFER, m_RenderBuffer);
                glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_Width, m_Height);                         // use a single renderbuffer object for both a depth AND stencil buffer.
                glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_RenderBuffer);// now actually attach it
                break;
            case FramebufferSpec::DEPTH_MAP:
                glGenTextures(1, &m_TextureBuffer);
                glBindTexture(GL_TEXTURE_2D, m_TextureBuffer);
                glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, m_Width, m_Height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
                float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };// no shadow for depth values outside the 1024x1024 texture
                glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_TextureBuffer, 0);
                glDrawBuffer(GL_NONE);
                glReadBuffer(GL_NONE);
                break;
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
    // now that we actually created the framebuffer and added all attachments we want to check if it is actually complete now
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

// generates a new framebuffer based on the new width and height
void Framebuffer::resize(unsigned int width, unsigned int height) {
    m_Framebuffer = 0;
    glGenFramebuffers(1, &m_Framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, m_Framebuffer);

    for (const auto &spec : m_Specs) {
        switch (spec) {
            case FramebufferSpec::COLOR:
                // create a color attachment texture
                glGenTextures(1, &m_TextureBuffer);
                glBindTexture(GL_TEXTURE_2D, m_TextureBuffer);
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_TextureBuffer, 0);
                break;
            case FramebufferSpec::DEPTH_MAP:
                // create a depth attachment texture
                glGenTextures(1, &m_TextureBuffer);
                glBindTexture(GL_TEXTURE_2D, m_TextureBuffer);
                glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, m_Width, m_Height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_TextureBuffer, 0);
                glDrawBuffer(GL_NONE);
                glReadBuffer(GL_NONE);
                break;
            case FramebufferSpec::DEPTH24STENCIL8:
                // create a renderbuffer object for depth and stencil attachment (we won't be sampling these)
                glGenRenderbuffers(1, &m_RenderBuffer);
                glBindRenderbuffer(GL_RENDERBUFFER, m_RenderBuffer);
                glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);                             // use a single renderbuffer object for both a depth AND stencil buffer.
                glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_RenderBuffer);// now actually attach it
                break;
        }
    }
    // now that we actually created the framebuffer and added all attachments we want to check if it is actually complete now
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}