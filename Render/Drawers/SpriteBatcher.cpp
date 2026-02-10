// Render/Drawers/SpriteBatcher.cpp
#include "SpriteBatcher.h"
#include "Render/Pipelines/SpritePipeline.h"
#include "RHI/DX11/DX11Common.h"
#include "RHI/DX11/DX11Device.h"
#include "RHI/DX11/DX11SwapChain.h"

#include <cmath>
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
        const float x = sprite.dstRect.x;
        const float y = sprite.dstRect.y;
        const float w = sprite.dstRect.w;
        const float h = sprite.dstRect.h;

        const float u0 = sprite.uv.u0;
        const float v0 = sprite.uv.v0;
        const float u1 = sprite.uv.u1;
        const float v1 = sprite.uv.v1;

        const float r = sprite.tint.r;
        const float g = sprite.tint.g;
        const float b = sprite.tint.b;
        const float a = sprite.tint.a;

        float x00, y00, x10, y10, x01, y01, x11, y11;
        if (!sprite.hasTransform) {
            x00 = x;     y00 = y;
            x10 = x + w; y10 = y;
            x01 = x;     y01 = y + h;
            x11 = x + w; y11 = y + h;
        } else {
            const float px = sprite.pivotX * w;
            const float py = sprite.pivotY * h;
            const float sx = sprite.scaleX;
            const float sy = sprite.scaleY;

            const float cosR = std::cos(sprite.rotRad);
            const float sinR = std::sin(sprite.rotRad);

            auto Transform = [&](float localX, float localY, float& outX, float& outY) {
                // Apply pivot and scale
                float tx = (localX - px) * sx;
                float ty = (localY - py) * sy;

                // Apply rotation
                float rotX = tx * cosR - ty * sinR;
                float rotY = tx * sinR + ty * cosR;

                // Apply translation
                outX = x + rotX + px;
                outY = y + rotY + py;
            };
            
            Transform(0, 0, x00, y00);
            Transform(w, 0, x10, y10);
            Transform(0, h, x01, y01);
            Transform(w, h, x11, y11);
        }

        SpriteVertex quad[6] = {
            {{x00, y00}, {u0, v0}, {r, g, b, a}},
            {{x10, y10}, {u1, v0}, {r, g, b, a}},
            {{x01, y01}, {u0, v1}, {r, g, b, a}},

            {{x01, y01}, {u0, v1}, {r, g, b, a}},
            {{x10, y10}, {u1, v0}, {r, g, b, a}},
            {{x11, y11}, {u1, v1}, {r, g, b, a}},
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