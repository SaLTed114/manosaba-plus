// Render/Drawers/SpriteBatcher.h
#ifndef RENDER_DRAWERS_SPRITEBATCHER_H
#define RENDER_DRAWERS_SPRITEBATCHER_H

#include <cstdint>
#include <span>
#include <wrl/client.h>
#include <d3d11.h>

#include "Render/Draw/DrawList.h"
#include "Render/RenderPlan.h"

namespace Salt2D::RHI::DX11 {
    class DX11Device;
} // namespace Salt2D::RHI::DX11

namespace Salt2D::Render {

class SpriteBatcher {
public:
    void Initialize(const RHI::DX11::DX11Device& device);

    void DrawBatch(PassContext& ctx, std::span<const SpriteDrawItem> sprites);

private:
    struct SpriteVertex {
        float posPX[2];
        float uv[2];
        float color[4];
    };

    void EnsureVB(const RHI::DX11::DX11Device& device, size_t spriteCount);

private:
    Microsoft::WRL::ComPtr<ID3D11Buffer> vb_;
    size_t vbCapacity_ = 0;
};

} // namespace Salt2D::Render

#endif // RENDER_DRAWERS_SPRITEBATCHER_H
