// Render/RenderPlan.h
#ifndef RENDER_RENDERPLAN_H
#define RENDER_RENDERPLAN_H

#include <cstdint>
#include <functional>
#include <vector>
#include <d3d11.h>
#include "DX11CommonState.h"

namespace Salt2D::RHI::DX11 {
    class DX11Device;
} // namespace Salt2D::RHI::DX11

namespace Salt2D::Render {

class SpriteRenderer;
class ComposePipeline;
class CubeDemo;

enum class Target { Scene, BackBuffer };
enum class DepthMode { Off, RO, RW };
enum class BlendMode { Off, Alpha };

struct PassContext {
    RHI::DX11::DX11Device& device;
    ID3D11DeviceContext*   ctx;

    ID3D11RenderTargetView*   sceneRTV = nullptr;
    ID3D11ShaderResourceView* sceneSRV = nullptr;
    ID3D11DepthStencilView*   sceneDSV = nullptr;
    ID3D11RenderTargetView*   backRTV  = nullptr;

    uint32_t canvasW = 0, canvasH = 0;
    uint32_t sceneW  = 0, sceneH  = 0;

    DX11CommonState& states;
    DX11StateCache&  cache;

    SpriteRenderer*  spriteRenderer = nullptr;
    ComposePipeline* compose        = nullptr;
    CubeDemo*        cubeDemo       = nullptr;
};

struct RenderPass {
    const char* name = "Unnamed";
    Target      target = Target::Scene;

    bool clearColor = false;
    bool clearDepth = false;
    float clearColorValue[4] = {0, 0, 0, 1};
    float clearDepthValue = 1.0f;
    uint8_t clearStencilValue = 0;

    DepthMode depth = DepthMode::Off;
    BlendMode blend = BlendMode::Off;

    std::function<void(PassContext&)> exec;
};

struct RenderPlan {
    std::vector<RenderPass> passes;
    void Clear() { passes.clear(); }
};


// Helper function to get DepthStencilState from DepthMode
inline ID3D11DepthStencilState* DepthStencilState(DX11CommonState* states, DepthMode mode) {
    switch (mode) {
    case DepthMode::RO:  return states->depthRO.Get();
    case DepthMode::RW:  return states->depthRW.Get();
    case DepthMode::Off:
    default: return states->depthOff.Get();
    }
}

inline ID3D11BlendState* BlendState(DX11CommonState* states, BlendMode mode) {
    switch (mode) {
    case BlendMode::Alpha: return states->blendAlpha.Get();
    case BlendMode::Off:
    default: return states->blendOff.Get();
    }
}

} // namespace Salt2D::Render

#endif // RENDER_RENDERPLAN_H
