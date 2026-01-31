// Render/Draw/DrawList.h
#ifndef RENDER_DRAW_DRAWLIST_H
#define RENDER_DRAW_DRAWLIST_H

#include <cstdint>
#include <vector>
#include <algorithm>
#include <array>
#include <span>

#include "SpriteDrawItem.h"

namespace Salt2D::Render {

class DrawList {
public:
    struct Range {
        uint32_t begin = 0;
        uint32_t end = 0;
    }; // [begin, end)

    void Clear() {
        sprites_.clear();
        nextOrder_ = 0;
        for (auto& range : layerRanges_) range = {0, 0};
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

        RebuildLayerRanges();
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

    std::span<const SpriteDrawItem> SpritesAll() const {
        return { sprites_.data(), sprites_.size() };
    }

    std::span<const SpriteDrawItem> Sprites(Layer layer) const {
        const auto& range = layerRanges_[ToLayerIndex(layer)];
        return { sprites_.data() + range.begin, range.end - range.begin };
    }

    std::span<const SpriteDrawItem> Sprites(Layer minLayer, Layer maxLayer) const {
        const uint8_t minIdx = ToLayerIndex(minLayer);
        const uint8_t maxIdx = ToLayerIndex(maxLayer);
        if (minIdx > maxIdx) return {};

        uint32_t begin = static_cast<uint32_t>(sprites_.size());
        for (uint8_t i = minIdx; i <= maxIdx; ++i) {
            const auto& range = layerRanges_[i];
            if (range.begin < range.end) {
                begin = range.begin;
                break;
            }
        }

        uint32_t end = 0;
        for (uint8_t i = maxIdx; i != static_cast<uint8_t>(-1) && i >= minIdx; --i) {
            const auto& range = layerRanges_[i];
            if (range.begin < range.end) {
                end = range.end;
                break;
            }
        }

        if (begin >= end || end > sprites_.size()) return {};
        return { sprites_.data() + begin, end - begin };
    }

private:
    void RebuildLayerRanges() {
        for (auto& range : layerRanges_) range = {0, 0};

        const uint32_t spriteCount = static_cast<uint32_t>(sprites_.size());
        uint32_t i = 0;
        while (i < spriteCount) {
            Layer layer = sprites_[i].layer;
            const uint8_t layerIdx = ToLayerIndex(layer);

            uint32_t begin = i;
            while (i < spriteCount && sprites_[i].layer == layer) i++;
            uint32_t end = i;
            layerRanges_[layerIdx] = { begin, end };
        }
    }

private:
    std::vector<SpriteDrawItem> sprites_;
    uint32_t nextOrder_ = 0;

    std::array<Range, ToLayerIndex(Layer::Count)> layerRanges_;
};

} // namespace Salt2D::Render

#endif // RENDER_DRAW_DRAWLIST_H
