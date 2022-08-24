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

#define FRAMEBUFFER_BEGIN(buf) buf.bind()
#define FRAMEBUFFER_END()      glBindFramebuffer(GL_FRAMEBUFFER, 0)

class Framebuffer {
public:
    enum class FramebufferSpec {
        COLOR,
        DEPTH_MAP,
        DEPTH24STENCIL8,
    };

    Framebuffer();
    virtual ~Framebuffer();

    virtual void init(unsigned int width, unsigned int height) {
        m_Width  = width;
        m_Height = height;
        glGenFramebuffers(1, &m_Framebuffer);
    }

    virtual void add_spec(FramebufferSpec spec);
    virtual void create();
    virtual void resize(unsigned int width, unsigned int height);
    virtual void bind() const { glBindFramebuffer(GL_FRAMEBUFFER, m_Framebuffer); }

    inline unsigned int get_texture() const { return m_TextureBuffer; }

protected:
    // for specific framebuffers that ultimately need these (the create method will be overriden anyway)
    unsigned int m_Width;
    unsigned int m_Height;
    unsigned int m_Framebuffer;
    unsigned int m_TextureBuffer;
    unsigned int m_RenderBuffer;

private:
    std::vector<FramebufferSpec> m_Specs;
};