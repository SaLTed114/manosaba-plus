// Render/Passes/CubePass.cpp
#include "CubePass.h"
#include "Render/Demo/CubeDemo.h"

namespace Salt2D::Render {

CubePass::CubePass(const char* name, DirectX::XMMATRIX transform)
    : RenderPassBase(name, Target::Scene, DepthMode::RW, BlendMode::Off)
    , transform_(transform) {}

void CubePass::Execute(PassContext& ctx) {
    using namespace DirectX;
    XMMATRIX mvp = transform_ * ctx.frame->view * ctx.frame->proj;
    ctx.cubeDemo->Draw(ctx, mvp);
}

} // namespace Salt2D::Render
