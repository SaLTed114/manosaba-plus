// Render/Draw/DrawList.h
#ifndef RENDER_DRAW_DRAWLIST_H
#define RENDER_DRAW_DRAWLIST_H

#include <cstdint>
#include <vector>
#include <algorithm>

#include "DrawItem.h"

namespace Salt2D::Render {

class DrawList {
public:
    void Clear() {
        sprites_.clear();
        nextOrder_ = 0;
    }

    void ReserveSprites(size_t count) {
        sprites_.reserve(count);
    }

    void Sort() {
        std::stable_sort(sprites_.begin(), sprites_.end(),
            [](const SpriteDrawItem& a, const SpriteDrawItem& b) {
                if (a.layer != b.layer) return ToLayerIndex(a.layer) < ToLayerIndex(b.layer);
                if (a.z != b.z) return a.z < b.z;
                return a.order < b.order;
            });
    }

    SpriteDrawItem& PushSprite(
        Layer layer,
        ID3D11ShaderResourceView* srv,
        const RectF& dstRect,
        float z = 0.0f,
        const UVRectF& uv = {},
        const Color4F& tint = {}
    ) {
        SpriteDrawItem item;
        item.srv     = srv;
        item.dstRect = dstRect;
        item.uv      = uv;
        item.tint    = tint;
        item.layer   = layer;
        item.z       = z;
        item.order   = nextOrder_++;

        sprites_.push_back(item);
        return sprites_.back();
    }

    const std::vector<SpriteDrawItem>& Sprites() const { return sprites_; }
    std::vector<SpriteDrawItem>& Sprites() { return sprites_; }

private:
    std::vector<SpriteDrawItem> sprites_;
    uint32_t nextOrder_ = 0;
};

} // namespace Salt2D::Render

#endif // RENDER_DRAW_DRAWLIST_H
