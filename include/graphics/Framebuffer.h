#pragma once

#include "common.h"
#include <glad/glad.h>

// usage:
// Framebuffer buffer(1024, 860);
// buffer.add_spec(FramebufferSpec::TEXTURE);
// buffer.add_spec(FramebufferSpec::DEPTH24STENCIL8);
// buffer.create();
//
// render loop:
// 	draw_image(buffer.get_texture());
// 	on window resize:
// 		buffer.resize(width, height);
class Framebuffer {
public:
    enum class FramebufferSpec {
        TEXTURE,
        DEPTH24STENCIL8,
    };

    Framebuffer();
    ~Framebuffer();

    void init(unsigned int width, unsigned int height) {
        m_Width  = width;
        m_Height = height;
        glGenFramebuffers(1, &m_Framebuffer);
    }
    void add_spec(FramebufferSpec spec);
    void create();
    void resize(unsigned int width, unsigned int height);
    void bind() const { glBindFramebuffer(GL_FRAMEBUFFER, m_Framebuffer); }

    inline unsigned int get_texture() const { return m_TextureBuffer; }

private:
    unsigned int m_Width, m_Height;
    unsigned int m_Framebuffer;
    unsigned int m_TextureBuffer;
    unsigned int m_RenderBuffer;

    std::vector<FramebufferSpec> m_Specs;
};