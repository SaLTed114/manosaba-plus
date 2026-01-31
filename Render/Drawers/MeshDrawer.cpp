// Render/Drawers/MeshDrawer.cpp
#include "MeshDrawer.h"
#include "Render/Pipelines/MeshPipeline.h"
#include "Render/RenderPlan.h"
#include "RHI/DX11/DX11Device.h"

#include <stdexcept>

namespace Salt2D::Render {

void MeshDrawer::Initialize(const RHI::DX11::DX11Device& /*device*/) {
    // placeholder for future use
}

void MeshDrawer::DrawBatch(PassContext& ctx, std::span<const MeshDrawItem> meshes) {
    if (meshes.empty()) return;

    auto& pipeline = ctx.pipelines->Mesh();
    pipeline.Bind(ctx.ctx);

    for (const auto& item : meshes) {
        Draw(ctx, item);
    }
}

void MeshDrawer::DrawItem(PassContext& ctx, const MeshDrawItem& item) {
    auto& pipeline = ctx.pipelines->Mesh();
    pipeline.Bind(ctx.ctx);
    Draw(ctx, item);
}

void MeshDrawer::Draw(PassContext& ctx, const MeshDrawItem& item) {
    if (!item.mesh || !item.mesh->vb || !item.mesh->ib) return;

    auto wvp = item.world * ctx.frame->viewProj;

    auto& pipeline = ctx.pipelines->Mesh();

    pipeline.SetConstants(ctx.ctx, wvp);

    UINT stride = item.mesh->stride;
    UINT offset = 0;
    ID3D11Buffer* vbArray[] = { item.mesh->vb.Get() };
    ctx.ctx->IASetVertexBuffers(0, 1, vbArray, &stride, &offset);
    ctx.ctx->IASetIndexBuffer(item.mesh->ib.Get(), item.mesh->indexFormat, 0);
    ctx.ctx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    ctx.ctx->DrawIndexed(item.mesh->indexCount, 0, 0);
}

} // namespace Salt2D::Render
