// Render/Sprite/SpriteRenderer.cpp
#include "SpriteRenderer.h"
#include "Render/Shader/ShaderManager.h"
#include "RHI/DX11/DX11Common.h"
#include "RHI/DX11/DX11Device.h"
#include "RHI/DX11/DX11SwapChain.h"

#include <stdexcept>

namespace Salt2D::Render {

void SpriteRenderer::Initialize(const RHI::DX11::DX11Device& device, ShaderManager& shaderManager) {
    pipeline_.Initialize(device, shaderManager);
    EnsureVB(device, 1024);
}

void SpriteRenderer::EnsureVB(const RHI::DX11::DX11Device& device, size_t spriteCount) {
    if (spriteCount <= vbCapacity_ && vb_) return;

    auto d3dDevice = device.GetDevice();

    vbCapacity_ = (spriteCount < 1024) ? 1024 : spriteCount;

    const UINT vertexCount = static_cast<UINT>(vbCapacity_ * 6);
    const UINT byteSize = vertexCount * sizeof(Vertex);

    D3D11_BUFFER_DESC vbDesc{};
    vbDesc.ByteWidth = byteSize;
    vbDesc.Usage = D3D11_USAGE_DYNAMIC;
    vbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

    ThrowIfFailed(d3dDevice->CreateBuffer(&vbDesc, nullptr, vb_.GetAddressOf()),
        "SpriteRenderer::EnsureVB: CreateBuffer for vertex buffer failed.");
}

void SpriteRenderer::Draw(
    const RHI::DX11::DX11Device& device,
    const DrawList& drawList,
    uint32_t screenW, uint32_t screenH
) {
    const auto& sprites = drawList.Sprites();
    if (sprites.empty()) return;

    if (sprites.size() > vbCapacity_) EnsureVB(device, sprites.size());

    auto context = device.GetContext();

    D3D11_MAPPED_SUBRESOURCE mappedResource;
    ThrowIfFailed(context->Map(vb_.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource),
        "SpriteRenderer::Draw: Map vertex buffer failed.");

    auto* vtx = reinterpret_cast<Vertex*>(mappedResource.pData);

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

        Vertex quad[6] = {
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

    context->Unmap(vb_.Get(), 0);

    pipeline_.Bind(context);
    pipeline_.SetFrameConstants(context, screenW, screenH);

    const UINT stride = sizeof(Vertex);
    const UINT offset = 0;
    ID3D11Buffer* vbs[] = { vb_.Get() };
    context->IASetVertexBuffers(0, 1, vbs, &stride, &offset);
    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    const auto spriteCount = static_cast<UINT>(sprites.size());
    for (UINT i = 0; i < spriteCount; ++i) {
        pipeline_.BindTexture(context, sprites[i].srv);
        context->Draw(6, i * 6);
    }

    ID3D11ShaderResourceView* nullSRV[] = { nullptr };
    context->PSSetShaderResources(0, 1, nullSRV);
}

} // namespace Salt2D::Render
