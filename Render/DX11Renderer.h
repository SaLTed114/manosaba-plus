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
#include "Render/Sprite/SpriteRenderer.h"

#include "Render/Pipeline/ComposePipeline.h"

#include "Render/Primitives/CubeDemo.h"

namespace Salt2D::Render {

class DX11Renderer {
public:
    DX11Renderer(HWND hwnd, uint32_t width, uint32_t height);

    void Resize(uint32_t width, uint32_t height);

    void RenderFrame(bool vsync = true);

private:
    void InitShaderSearchPaths();
    void InitSceneTargets(float factor);
    void InitPipelines();
    void InitStates();
    void InitDemoTextures(); 

private:
    void BuildTestDrawList(); // TMP
    void Present(bool vsync);

private:
    RenderPlan BuildDefaultPlan();
    void ExecutePlan(const RenderPlan& plan);

private:
    uint32_t canvasW_ = 0;
    uint32_t canvasH_ = 0;
    uint32_t sceneW_  = 0;
    uint32_t sceneH_  = 0;
    float internalScale_ = 1.0f;

    RHI::DX11::DX11Device       device_;
    RHI::DX11::DX11SwapChain    swapChain_;
    RHI::DX11::DX11DepthBuffer  sceneDepth_;

    DX11CommonState states_;
    DX11StateCache  stateCache_;

    ShaderManager     shaderManager_;
    DrawList          drawList_;
    SpriteRenderer    spriteRenderer_;

    RHI::DX11::DX11Texture2D testTexture_;
    RHI::DX11::DX11Texture2D imgTexture_;

    RHI::DX11::DX11Texture2D sceneRT_;
    ComposePipeline compose_;

    CubeDemo cubeDemo_;
};

} // namespace Salt2D::Render

#endif // RENDER_DX11RENDERER_H
