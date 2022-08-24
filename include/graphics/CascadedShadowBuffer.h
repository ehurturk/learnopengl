#pragma once

#include "Framebuffer.h"
#include "common.h"

class CascadedShadowBuffer : public Framebuffer {
public:
    CascadedShadowBuffer(float near, float far);
    ~CascadedShadowBuffer();

    void create() override;
    void resize(ui32 width, ui32 height) override;

private:
    float near;
    float far;
    std::vector<float> cascade_levels{ far / 50.0f, far / 25.0f, far / 10.0f, far / 2.0f };
};