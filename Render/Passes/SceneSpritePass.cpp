// Render/Passes/SceneSpritePass.cpp
#include "SceneSpritePass.h"
#include "Render/Renderers/SpriteRenderer.h"

namespace Salt2D::Render {

SpritePass::SpritePass(
    const char* name, Target target,
    DepthMode depth, BlendMode blend,
    std::span<const SpriteDrawItem> sprites
) : RenderPassBase(name, target, depth, blend), sprites_(sprites) {}

void SpritePass::SetClearScene(float r, float g, float b, float a) {
    clearColor_ = true; clearDepth_ = true;
    clearColorValue_[0] = r; clearColorValue_[1] = g;
    clearColorValue_[2] = b; clearColorValue_[3] = a;
}

void SpritePass::Execute(PassContext& ctx) {
    ctx.spriteRenderer->Draw(ctx.device, sprites_, ctx.canvasW, ctx.canvasH);
}

} // namespace Salt2D::Render
