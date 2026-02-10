// Render/Draw/SpriteDrawItem.h
#ifndef RENDER_DRAW_SPRITEDRAWITEM_H
#define RENDER_DRAW_SPRITEDRAWITEM_H

#include <cstdint>

struct ID3D11ShaderResourceView;

namespace Salt2D::Render {

enum class Layer : uint8_t {
    Background = 0,
    Stage      = 1,
    Text       = 2,
    HUD        = 3,
    Count      = 4,
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

struct RectI {
    int32_t l = 0;
    int32_t t = 0;
    int32_t r = 0;
    int32_t b = 0;
};

struct SpriteDrawItem {
    ID3D11ShaderResourceView* srv = nullptr;

    RectF   dstRect;
    UVRectF uv;
    Color4F tint;

    bool clipEnabled = false;
    RectI clipRect;

    Layer layer = Layer::Stage;
    float z = 0.0f;
    uint32_t order = 0;

    // 2D transform
    bool hasTransform = false;
    float rotRad = 0.0f;
    float scaleX = 1.0f;
    float scaleY = 1.0f;
    float pivotX = 0.0f; // 0: left, 0.5: center, 1: right
    float pivotY = 0.0f; // 0: top,  0.5: center, 1: bottom
};

constexpr static inline uint8_t ToLayerIndex(Layer layer) {
    return static_cast<uint8_t>(layer);
}

} // namespace Salt2D::Render

#endif // RENDER_DRAW_SPRITEDRAWITEM_H
