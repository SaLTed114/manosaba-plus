// Render/Drawers/MeshDrawer.cpp
#include "MeshDrawer.h"
#include "Render/Pipelines/MeshPipeline.h"
#include "Render/RenderPlan.h"
#include "RHI/DX11/DX11Device.h"

namespace Salt2D::Render {

void MeshDrawer::Initialize(const RHI::DX11::DX11Device& device) {
    (void)device; // placeholder for future use
}

void MeshDrawer::DrawMesh(
    PassContext& ctx,
    ID3D11Buffer* vertexBuffer,
    ID3D11Buffer* indexBuffer,
    uint32_t indexCount,
    const DirectX::XMMATRIX& worldViewProj
) {
    auto& pipeline = ctx.pipelines->Mesh();
    auto context = ctx.ctx;

    pipeline.Bind(context);
    pipeline.SetConstants(context, worldViewProj);

    UINT stride = sizeof(MeshPipeline::VertexCPU);
    UINT offset = 0;
    ID3D11Buffer* vbArray[] = { vertexBuffer };
    context->IASetVertexBuffers(0, 1, vbArray, &stride, &offset);
    context->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R16_UINT, 0);
    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    context->DrawIndexed(indexCount, 0, 0);
}

} // namespace Salt2D::Render
