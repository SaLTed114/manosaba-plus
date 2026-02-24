// Render/DX11Renderer.h
#ifndef RENDER_DX11RENDERER_H
#define RENDER_DX11RENDERER_H

#include <cstdint>
#include <wrl/client.h>

#include "RHI/DX11/DX11Device.h"
#include "RHI/DX11/DX11SwapChain.h"
#include "RHI/DX11/DX11Texture2D.h"
#include "RHI/DX11/DX11DepthBuffer.h"
#include "Render/DX11CommonState.h"
#include "Render/RenderPlan.h"
#include "Render/Shader/ShaderManager.h"
#include "Render/Draw/DrawList.h"

#include "Render/Pipelines/PipelineLibrary.h"
#include "Render/Drawers/DrawServices.h"
#include "Render/DX11DebugLogger.h"

namespace Salt2D::Render {

class DX11Renderer {
public:
    DX11Renderer(HWND hwnd, uint32_t width, uint32_t height);
    ~DX11Renderer();

    void Resize(uint32_t width, uint32_t height);

    void ExecutePlan(const RenderPlan& plan, const FrameBlackboard& frame);
    void Present(bool vsync);

    const RHI::DX11::DX11Device& Device() const { return device_; }
    const uint32_t GetCanvasW() const { return canvasW_; }
    const uint32_t GetCanvasH() const { return canvasH_; }
    const uint32_t GetSceneW()  const { return sceneW_; }
    const uint32_t GetSceneH()  const { return sceneH_; }

    void FlushDebugMessages();

private:
    void InitShaderSearchPaths();
    void InitSceneTargets(float factor);
    void InitDebugLayer();

private:
    uint32_t canvasW_ = 0;
    uint32_t canvasH_ = 0;
    uint32_t sceneW_  = 0;
    uint32_t sceneH_  = 0;
    float internalScale_ = 1.0f;

    RHI::DX11::DX11Device       device_;
    RHI::DX11::DX11SwapChain    swapChain_;
    RHI::DX11::DX11DepthBuffer  sceneDepth_;

    ShaderManager shaderManager_;

    DX11CommonState states_;
    DX11StateCache  stateCache_;

    RHI::DX11::DX11Texture2D sceneRT_[2];
    uint32_t sceneIdx_ = 0;
    uint32_t snapshotIdx_ = 1;  // 第一帧快照的缓冲区索引
    bool sceneValid_ = false;

    PipelineLibrary pipelines_;
    DrawServices draw_;

    DX11DebugLogger debugLogger_;
};

} // namespace Salt2D::Render

#endif // RENDER_DX11RENDERER_H
