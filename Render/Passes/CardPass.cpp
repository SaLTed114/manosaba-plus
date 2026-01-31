// Render/Passes/CardPass.cpp
#include "CardPass.h"
#include "Render/Drawers/CardDrawer.h"

namespace Salt2D::Render {

CardPass::CardPass(
    const char* name, Target target,
    DepthMode depth, BlendMode blend,
    std::span<const CardDrawItem> cards
) : RenderPassBase(name, target, depth, blend), cards_(cards) {}

void CardPass::SetClearDepth(float depth, uint8_t stencil) {
    clearDepth_ = true;
    clearDepthValue_ = depth;
    clearStencilValue_ = stencil;
}

void CardPass::Execute(PassContext& ctx) {
    ctx.draw->Card().DrawBatch(ctx, cards_);
}

} // namespace Salt2D::Render
