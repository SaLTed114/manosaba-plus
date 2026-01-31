// Render/Drawers/CardDrawer.cpp
#include "CardDrawer.h"
#include "RHI/DX11/DX11Common.h"
#include "RHI/DX11/DX11Device.h"
#include "Render/Pipelines/PipelineLibrary.h"
#include "Render/Pipelines/CardPipeline.h"

#include <stdexcept>

namespace Salt2D::Render {

void CardDrawer::Initialize(const RHI::DX11::DX11Device& device) {
    auto d3dDevice = device.GetDevice();

    CardVertex vertices[4] = {
        { {-0.5f, 0.0f}, {0.0f, 1.0f} },
        { {+0.5f, 0.0f}, {1.0f, 1.0f} },
        { {+0.5f, 1.0f}, {1.0f, 0.0f} },
        { {-0.5f, 1.0f}, {0.0f, 0.0f} },
    };
    uint16_t indices[6] = { 0,1,2, 0,2,3 };

    D3D11_BUFFER_DESC vbDesc{};
    vbDesc.ByteWidth = sizeof(vertices);
    vbDesc.Usage = D3D11_USAGE_IMMUTABLE;
    vbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

    D3D11_SUBRESOURCE_DATA vbData{};
    vbData.pSysMem = vertices;
    ThrowIfFailed(d3dDevice->CreateBuffer(&vbDesc, &vbData, vb_.GetAddressOf()),
        "CardDrawer::Initialize: CreateBuffer for vertex buffer failed.");

    D3D11_BUFFER_DESC ibDesc{};
    ibDesc.ByteWidth = sizeof(indices);
    ibDesc.Usage = D3D11_USAGE_IMMUTABLE;
    ibDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

    D3D11_SUBRESOURCE_DATA ibData{};
    ibData.pSysMem = indices;
    ThrowIfFailed(d3dDevice->CreateBuffer(&ibDesc, &ibData, ib_.GetAddressOf()),
        "CardDrawer::Initialize: CreateBuffer for index buffer failed.");
}

void CardDrawer::DrawBatch(PassContext& ctx, std::span<const CardDrawItem> cards) {
    if (cards.empty()) return;

    auto& pipeline = ctx.pipelines->Card();
    pipeline.Bind(ctx.ctx);

    pipeline.SetFrameConstants(ctx.ctx, ctx.frame->viewProj);

    const UINT stride = sizeof(CardVertex);
    const UINT offset = 0;
    ID3D11Buffer* vbArray[] = { vb_.Get() };
    ctx.ctx->IASetVertexBuffers(0, 1, vbArray, &stride, &offset);
    ctx.ctx->IASetIndexBuffer(ib_.Get(), DXGI_FORMAT_R16_UINT, 0);
    ctx.ctx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    for (const auto& card : cards) {
        pipeline.SetCardConstants(ctx.ctx,
            card.pos, card.yaw,
            card.size, card.alphaCut);

        pipeline.BindTexture(ctx.ctx, card.srv);
        ctx.ctx->DrawIndexed(6, 0, 0);
    }

    ID3D11ShaderResourceView* nullSRV[] = { nullptr };
    ctx.ctx->PSSetShaderResources(0, 1, nullSRV);
}

} // namespace Salt2D::Render