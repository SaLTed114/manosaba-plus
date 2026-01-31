// Render/DX11CommonState.cpp
#include "DX11CommonState.h"
#include "RHI/DX11/DX11Common.h"

namespace Salt2D::Render {

void DX11CommonState::Initialize(ID3D11Device* device) {
    // Depth OFF
    {
        D3D11_DEPTH_STENCIL_DESC desc = {};
        desc.DepthEnable = FALSE;
        desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
        desc.DepthFunc = D3D11_COMPARISON_ALWAYS;
        desc.StencilEnable = FALSE;

        ThrowIfFailed(device->CreateDepthStencilState(&desc, depthOff.GetAddressOf()),
            "DX11CommonState::Initialize: CreateDepthStencilState (depthOff) failed.");
    }
    
    // Depth Read/Write
    {
        D3D11_DEPTH_STENCIL_DESC desc = {};
        desc.DepthEnable = TRUE;
        desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
        desc.DepthFunc = D3D11_COMPARISON_LESS;
        desc.StencilEnable = FALSE;

        ThrowIfFailed(device->CreateDepthStencilState(&desc, depthRW.GetAddressOf()),
            "DX11CommonState::Initialize: CreateDepthStencilState (depthRW) failed.");
    }

    // Depth Read-Only
    {
        D3D11_DEPTH_STENCIL_DESC desc = {};
        desc.DepthEnable = TRUE;
        desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
        desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
        desc.StencilEnable = FALSE;

        ThrowIfFailed(device->CreateDepthStencilState(&desc, depthRO.GetAddressOf()),
            "DX11CommonState::Initialize: CreateDepthStencilState (depthRO) failed.");
    }

    // Blend OFF
    {
        D3D11_BLEND_DESC desc = {};
        desc.AlphaToCoverageEnable = FALSE;
        desc.IndependentBlendEnable = FALSE;

        auto& rtDesc = desc.RenderTarget[0];
        rtDesc.BlendEnable = FALSE;
        rtDesc.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

        ThrowIfFailed(device->CreateBlendState(&desc, blendOff.GetAddressOf()),
            "DX11CommonState::Initialize: CreateBlendState (blendOff) failed.");
    }

    // Blend Alpha
    {
        D3D11_BLEND_DESC desc = {};
        desc.AlphaToCoverageEnable = FALSE;
        desc.IndependentBlendEnable = FALSE;

        auto& rtDesc = desc.RenderTarget[0];
        rtDesc.BlendEnable = TRUE;
        rtDesc.SrcBlend = D3D11_BLEND_SRC_ALPHA;
        rtDesc.DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
        rtDesc.BlendOp = D3D11_BLEND_OP_ADD;
        rtDesc.SrcBlendAlpha = D3D11_BLEND_ONE;
        rtDesc.DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
        rtDesc.BlendOpAlpha = D3D11_BLEND_OP_ADD;
        rtDesc.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

        ThrowIfFailed(device->CreateBlendState(&desc, blendAlpha.GetAddressOf()),
            "DX11CommonState::Initialize: CreateBlendState (blendAlpha) failed.");
    }
}

} // namespace Salt2D::Render
