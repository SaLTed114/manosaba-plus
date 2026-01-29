// Render/Passes/RenderPassBase.h
#ifndef RENDER_PASSES_RENDERPASSBASE_H
#define RENDER_PASSES_RENDERPASSBASE_H

#include "IRenderPass.h"
#include "Render/RenderPlan.h"
#include "Render/DX11CommonState.h"
#include "Render/DX11RenderUtils.h"

namespace Salt2D::Render {

class RenderPassBase : public IRenderPass {
public:
    RenderPassBase(
        const char* name, Target target,
        DepthMode depth, BlendMode blend
    ) : name_(name), target_(target), depth_(depth), blend_(blend) {}

    std::string_view Name() const override { return name_; }

    void Record(PassContext& ctx) override {
        BindTargetAndViewport(ctx);
        ClearIfNeeded(ctx);
        ApplyStates(ctx);
        Execute(ctx);
    }

protected:
    virtual void Execute(PassContext& ctx) = 0;

    bool clearColor_ = false;
    bool clearDepth_ = false;
    float clearColorValue_[4] = {0, 0, 0, 1};
    float clearDepthValue_ = 1.0f;
    uint8_t clearStencilValue_ = 0;

private:
    void BindTargetAndViewport(PassContext& ctx) {
        if (target_ == Target::Scene) {
            ID3D11RenderTargetView* rtvs[] = { ctx.sceneRTV };
            ctx.ctx->OMSetRenderTargets(1, rtvs, ctx.sceneDSV);

            auto vp = MakeViewport(ctx.sceneW, ctx.sceneH);
            ctx.ctx->RSSetViewports(1, &vp);
        } else /*if (target == Target::BackBuffer)*/ {
            ID3D11RenderTargetView* rtvs[] = { ctx.backRTV };
            ctx.ctx->OMSetRenderTargets(1, rtvs, nullptr);

            auto vp = MakeViewport(ctx.canvasW, ctx.canvasH);
            ctx.ctx->RSSetViewports(1, &vp);
        }
    }

    void ClearIfNeeded(PassContext& ctx) {
        if (clearColor_) {
            auto rtv = (target_ == Target::Scene) ? ctx.sceneRTV : ctx.backRTV;
            ctx.ctx->ClearRenderTargetView(rtv, clearColorValue_);
        }
        if (clearDepth_ && target_ == Target::Scene && ctx.sceneDSV) {
            ctx.ctx->ClearDepthStencilView(ctx.sceneDSV, 
                D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
                clearDepthValue_, clearStencilValue_);
        }
    }

    void ApplyStates(PassContext& ctx) {
        ctx.cache.SetDepthStencilState(ctx.ctx, DepthStencilState(&ctx.states, depth_));
        ctx.cache.SetBlendState(ctx.ctx, BlendState(&ctx.states, blend_));
    }

private:
    const char* name_;
    Target      target_;
    DepthMode   depth_;
    BlendMode   blend_;
};
    
} // namespace Salt2D::Render

#endif // RENDER_PASSES_RENDERPASSBASE_H
