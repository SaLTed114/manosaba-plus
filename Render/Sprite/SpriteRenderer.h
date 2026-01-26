// Render/Sprite/SpriteRenderer.h
#ifndef RENDER_SPRITE_SPRITERENDERER_H
#define RENDER_SPRITE_SPRITERENDERER_H

#include <cstdint>
#include <wrl/client.h>
#include <d3d11.h>

#include "Render/Pipeline/SpritePipeline.h"
#include "Render/Draw/DrawList.h"

namespace Salt2D::RHI::DX11 {
    class DX11Device;
    class DX11SwapChain;
} // namespace Salt2D::RHI::DX11

namespace Salt2D::Render {

class ShaderManager;

class SpriteRenderer {
public:
    void Initialize(const RHI::DX11::DX11Device& device, ShaderManager& shaderManager);

    void Draw(
        const RHI::DX11::DX11Device& device,
        const DrawList& drawList,
        uint32_t screenW, uint32_t screenH
    );

private:
    struct Vertex {
        float posPX[2];
        float uv[2];
        float color[4];
    };

    void EnsureVB(const RHI::DX11::DX11Device& device, size_t spriteCount);

private:
    SpritePipeline pipeline_;
    Microsoft::WRL::ComPtr<ID3D11Buffer> vb_;
    size_t vbCapacity_ = 0;
};

} // namespace Salt2D::Render

#endif // RENDER_SPRITE_SPRITERENDERER_H
