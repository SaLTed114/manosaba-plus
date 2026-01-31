// Render/Passes/ComposePass.cpp
#include "ComposePass.h"
#include "Render/Pipelines/ComposePipeline.h"

namespace Salt2D::Render {

ComposePass::ComposePass(const char* name)
    : RenderPassBase(name, Target::BackBuffer, DepthMode::Off, BlendMode::Off) {}

void ComposePass::Execute(PassContext& ctx) {
    auto compose = ctx.pipelines->Compose();
    compose.Bind(ctx.ctx);

    ID3D11ShaderResourceView* srvs[] = { ctx.sceneSRV };
    ctx.ctx->PSSetShaderResources(0, 1, srvs);
    ctx.ctx->Draw(3, 0);

    ID3D11ShaderResourceView* nullSRV[] = { nullptr };
    ctx.ctx->PSSetShaderResources(0, 1, nullSRV);
}

} // namespace Salt2D::Render
