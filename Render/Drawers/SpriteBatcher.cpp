// Render/Drawers/SpriteBatcher.cpp
#include "SpriteBatcher.h"
#include "Render/Pipelines/SpritePipeline.h"
#include "RHI/DX11/DX11Common.h"
#include "RHI/DX11/DX11Device.h"
#include "RHI/DX11/DX11SwapChain.h"

#include <stdexcept>

// tmp
const size_t kMinCapacity = 1024;

namespace Salt2D::Render {

void SpriteBatcher::Initialize(const RHI::DX11::DX11Device& device) {
    EnsureVB(device, kMinCapacity);
}

void SpriteBatcher::EnsureVB(const RHI::DX11::DX11Device& device, size_t spriteCount) {
    if (spriteCount <= vbCapacity_ && vb_) return;

    auto d3dDevice = device.GetDevice();

    vbCapacity_ = (std::max)(spriteCount, kMinCapacity);

    const UINT vertexCount = static_cast<UINT>(vbCapacity_ * 6);
    const UINT byteSize = vertexCount * sizeof(SpriteVertex);

    D3D11_BUFFER_DESC vbDesc{};
    vbDesc.ByteWidth = byteSize;
    vbDesc.Usage = D3D11_USAGE_DYNAMIC;
    vbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

    ThrowIfFailed(d3dDevice->CreateBuffer(&vbDesc, nullptr, vb_.GetAddressOf()),
        "SpriteBatcher::EnsureVB: CreateBuffer for vertex buffer failed.");
}

void SpriteBatcher::DrawBatch(PassContext& ctx, std::span<const SpriteDrawItem> sprites) {
    if (sprites.empty()) return;

    if (sprites.size() > vbCapacity_) EnsureVB(ctx.device, sprites.size());

    D3D11_MAPPED_SUBRESOURCE mappedResource;

    ThrowIfFailed(ctx.ctx->Map(vb_.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource),
        "SpriteBatcher::DrawBatch: Map vertex buffer failed.");

    auto* vtx = reinterpret_cast<SpriteVertex*>(mappedResource.pData);

    for (const auto& sprite : sprites) {
        const float x0 = sprite.dstRect.x;
        const float y0 = sprite.dstRect.y;
        const float x1 = sprite.dstRect.x + sprite.dstRect.w;
        const float y1 = sprite.dstRect.y + sprite.dstRect.h;

        const float u0 = sprite.uv.u0;
        const float v0 = sprite.uv.v0;
        const float u1 = sprite.uv.u1;
        const float v1 = sprite.uv.v1;

        const float r = sprite.tint.r;
        const float g = sprite.tint.g;
        const float b = sprite.tint.b;
        const float a = sprite.tint.a;

        SpriteVertex quad[6] = {
            {{x0, y0}, {u0, v0}, {r, g, b, a}},
            {{x1, y0}, {u1, v0}, {r, g, b, a}},
            {{x0, y1}, {u0, v1}, {r, g, b, a}},

            {{x0, y1}, {u0, v1}, {r, g, b, a}},
            {{x1, y0}, {u1, v0}, {r, g, b, a}},
            {{x1, y1}, {u1, v1}, {r, g, b, a}},
        };

        memcpy(vtx, quad, sizeof(quad));
        vtx += 6;
    }

    ctx.ctx->Unmap(vb_.Get(), 0);

    auto& pipeline = ctx.pipelines->Sprite();
    pipeline.Bind(ctx.ctx);
    pipeline.SetFrameConstants(ctx.ctx, ctx.canvasW, ctx.canvasH);

    const UINT stride = sizeof(SpriteVertex);
    const UINT offset = 0;
    ID3D11Buffer* vbs[] = { vb_.Get() };
    ctx.ctx->IASetVertexBuffers(0, 1, vbs, &stride, &offset);
    ctx.ctx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    D3D11_RECT full;
    full.left   = 0;
    full.top    = 0;
    full.right  = static_cast<LONG>(ctx.canvasW);
    full.bottom = static_cast<LONG>(ctx.canvasH);

    auto rectEq = [](const D3D11_RECT& a, const D3D11_RECT& b) {
        return a.left == b.left && a.top == b.top && a.right == b.right && a.bottom == b.bottom;
    };

    D3D11_RECT currentRect = full;
    ctx.ctx->RSSetScissorRects(1, &currentRect);

    const auto spriteCount = static_cast<UINT>(sprites.size());
    for (UINT i = 0; i < spriteCount; i++) {
        D3D11_RECT scissorRect = full;
        if (sprites[i].clipEnabled) {
            scissorRect.left   = static_cast<LONG>(sprites[i].clipRect.l);
            scissorRect.top    = static_cast<LONG>(sprites[i].clipRect.t);
            scissorRect.right  = static_cast<LONG>(sprites[i].clipRect.r);
            scissorRect.bottom = static_cast<LONG>(sprites[i].clipRect.b);
        }

        if (!rectEq(scissorRect, currentRect)) {
            ctx.ctx->RSSetScissorRects(1, &scissorRect);
            currentRect = scissorRect;
        }

        pipeline.BindTexture(ctx.ctx, sprites[i].srv);
        ctx.ctx->Draw(6, i * 6);
    }
    
    ID3D11ShaderResourceView* nullSRV[] = { nullptr };
    ctx.ctx->PSSetShaderResources(0, 1, nullSRV);
}

} // namespace Salt2D::Render