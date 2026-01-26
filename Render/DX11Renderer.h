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
#include "Render/Shader/ShaderManager.h"
#include "Render/Draw/DrawList.h"
#include "Render/Sprite/SpriteRenderer.h"

#include "Render/Pipeline/ComposePipeline.h"

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
    void BeginScenePass();
    void ComposeToBackBuffer();
    void BeginHUDPass();
    void Present(bool vsync);

private:
    enum class DepthMode { Off, RO, RW };
    enum class BlendMode { Off, Alpha };

    ID3D11DepthStencilState* DS(DepthMode mode) {
        switch (mode) {
        case DepthMode::RO:  return states_.depthRO.Get();
        case DepthMode::RW:  return states_.depthRW.Get();
        case DepthMode::Off:
        default: return states_.depthOff.Get();
        }
    }

    ID3D11BlendState* BS(BlendMode mode) {
        switch (mode) {
        case BlendMode::Alpha: return states_.blendAlpha.Get();
        case BlendMode::Off:
        default: return states_.blendOff.Get();
        }
    }

    void BeginPass(
        ID3D11RenderTargetView* rtv,
        ID3D11DepthStencilView* dsv,
        uint32_t vpW, uint32_t vpH,
        DepthMode depthMode, BlendMode blendMode
    );

private:
    uint32_t width_  = 0;
    uint32_t height_ = 0;

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
    uint32_t internalW_ = 0;
    uint32_t internalH_ = 0;
};

} // namespace Salt2D::Render

#endif // RENDER_DX11RENDERER_H
