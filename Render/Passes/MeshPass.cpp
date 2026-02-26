// Render/Passes/MeshPass.cpp
#include "MeshPass.h"
#include "Render/Drawers/MeshDrawer.h"

namespace Salt2D::Render {

MeshPass::MeshPass(
    const char* name, Target target,
    DepthMode depth, BlendMode blend,
    std::span<const MeshDrawItem> meshes
) : RenderPassBase(name, target, depth, blend), meshes_(meshes) {}

void MeshPass::SetClearScene(float r, float g, float b, float a) {
    clearColor_ = true; clearDepth_ = true;
    clearColorValue_[0] = r; clearColorValue_[1] = g;
    clearColorValue_[2] = b; clearColorValue_[3] = a;
}

void MeshPass::SetClearDepth(float depth, uint8_t stencil) {
    clearDepth_ = true;
    clearDepthValue_ = depth;
    clearStencilValue_ = stencil;
}

void MeshPass::Execute(PassContext& ctx) {
    ctx.draw->Mesh().DrawBatch(ctx, meshes_);
}

} // namespace Salt2D::Render
