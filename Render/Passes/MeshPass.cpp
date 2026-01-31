// Render/Passes/MeshPass.cpp
#include "MeshPass.h"
#include "Render/Drawers/MeshDrawer.h"

namespace Salt2D::Render {

MeshPass::MeshPass(
    const char* name, Target target,
    DepthMode depth, BlendMode blend,
    std::span<const MeshDrawItem> meshes
) : RenderPassBase(name, target, depth, blend), meshes_(meshes) {}

void MeshPass::SetClearDepth(float depth, uint8_t stencil) {
    clearDepth_ = true;
    clearDepthValue_ = depth;
    clearStencilValue_ = stencil;
}

void MeshPass::Execute(PassContext& ctx) {
    ctx.draw->Mesh().DrawBatch(ctx, meshes_);
}

} // namespace Salt2D::Render
