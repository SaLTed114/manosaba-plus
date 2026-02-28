// Render/RenderPlan.h
#ifndef RENDER_RENDERPLAN_H
#define RENDER_RENDERPLAN_H

#include <cstdint>
#include <functional>
#include <vector>
#include <memory>
#include <d3d11.h>
#include <DirectXMath.h>
#include "DX11CommonState.h"
#include "Render/Passes/IRenderPass.h"
#include "Render/Pipelines/PipelineLibrary.h"
#include "Render/Drawers/DrawServices.h"

namespace Salt2D::RHI::DX11 {
    class DX11Device;
} // namespace Salt2D::RHI::DX11

namespace Salt2D::Render {

class ComposePipeline;

enum class Target { Scene, BackBuffer };
enum class DepthMode { Off, RO, RW };
enum class BlendMode { Off, Alpha };

struct FrameBlackboard {
    DirectX::XMMATRIX view;
    DirectX::XMMATRIX proj;
    DirectX::XMMATRIX viewProj;

    // 0: fully previous scene, 1: fully current scene
    float sceneCrossfade = 1.0f;
    uint32_t lockPrevScene = 0;
    uint32_t pad[2] = {0, 0};
    
    // Global point light position for 3D rendering
    DirectX::XMFLOAT3 lightPosition = {0.0f, 10.0f, 0.0f};
};

inline FrameBlackboard DefaultFrameBlackboard() {
    FrameBlackboard fb{};
    fb.view = DirectX::XMMatrixIdentity();
    fb.proj = DirectX::XMMatrixIdentity();
    fb.viewProj = DirectX::XMMatrixIdentity();
    return fb;
}

struct PassContext {
    RHI::DX11::DX11Device& device;
    ID3D11DeviceContext*   ctx;

    ID3D11RenderTargetView*   sceneRTV = nullptr;
    ID3D11ShaderResourceView* sceneSRV = nullptr;
    ID3D11DepthStencilView*   sceneDSV = nullptr;
    ID3D11RenderTargetView*   backRTV  = nullptr;

    ID3D11ShaderResourceView* prevSceneSRV = nullptr;

    uint32_t canvasW = 0, canvasH = 0;
    uint32_t sceneW  = 0, sceneH  = 0;

    DX11CommonState& states;
    DX11StateCache&  cache;

    PipelineLibrary* pipelines = nullptr;
    DrawServices*    draw      = nullptr;

    const FrameBlackboard* frame = nullptr;
};

struct RenderPlan {
    std::vector<std::unique_ptr<IRenderPass>> passes;
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
