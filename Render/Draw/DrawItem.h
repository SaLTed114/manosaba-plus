// Render/Draw/DrawItem.h
#ifndef RENDER_DRAW_DRAWITEM_H
#define RENDER_DRAW_DRAWITEM_H

#include <cstdint>

struct ID3D11ShaderResourceView;

namespace Salt2D::Render {

enum class Layer : uint8_t {
    Background = 0,
    Stage      = 1,
    Text       = 2,
    HUD        = 3,
    Count
};

struct RectF {
    float x = 0.0f;
    float y = 0.0f;
    float w = 0.0f;
    float h = 0.0f;
};

struct UVRectF {
    float u0 = 0.0f;
    float v0 = 0.0f;
    float u1 = 1.0f;
    float v1 = 1.0f;
};

struct Color4F {
    float r = 1.0f;
    float g = 1.0f;
    float b = 1.0f;
    float a = 1.0f;
};

struct SpriteDrawItem {
    ID3D11ShaderResourceView* srv = nullptr;

    RectF   dstRect;
    UVRectF uv;
    Color4F tint;

    Layer layer = Layer::Stage;
    float z = 0.0f;
    uint32_t order = 0;
};

static inline uint8_t ToLayerIndex(Layer layer) {
    return static_cast<uint8_t>(layer);
}

} // namespace Salt2D::Render

#endif // RENDER_DRAW_DRAWITEM_H
