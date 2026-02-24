// Render/Passes/ComposePass.cpp
#include "ComposePass.h"
#include "Render/Pipelines/ComposePipeline.h"

namespace Salt2D::Render {

ComposePass::ComposePass(const char* name)
    : RenderPassBase(name, Target::BackBuffer, DepthMode::Off, BlendMode::Off) {}

void ComposePass::Execute(PassContext& ctx) {
    auto compose = ctx.pipelines->Compose();
    compose.Bind(ctx.ctx);

    compose.SetConstants(ctx.ctx, ctx.frame->sceneCrossfade);

    ID3D11ShaderResourceView* srvs[] = { ctx.sceneSRV, ctx.prevSceneSRV };
    ctx.ctx->PSSetShaderResources(0, 2, srvs);
    ctx.ctx->Draw(3, 0);

    ID3D11ShaderResourceView* nullSRV[] = { nullptr, nullptr };
    ctx.ctx->PSSetShaderResources(0, 2, nullSRV);
}

} // namespace Salt2D::Render
