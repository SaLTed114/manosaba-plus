// Render/Pipeline/SpritePipeline.cpp
#include "SpritePipeline.h"
#include "Render/Shader/ShaderManager.h"
#include "RHI/DX11/DX11Common.h"
#include "RHI/DX11/DX11Device.h"

#include <stdexcept>

namespace Salt2D::Render {

struct CBFrameData {
    float screenSize[2];
    float padding[2];
};

void SpritePipeline::Initialize(const RHI::DX11::DX11Device& device, ShaderManager& shaderManager) {
    auto d3dDevice = device.GetDevice();
    if (!d3dDevice) throw std::runtime_error("SpritePipeline::Initialize: device is null.");

    auto vsBlob = shaderManager.LoadShader("sprite_vs.cso");
    auto psBlob = shaderManager.LoadShader("sprite_ps.cso");
    if (!vsBlob || !psBlob) throw std::runtime_error("SpritePipeline::Initialize: Failed to load sprite shaders.");

    ThrowIfFailed(d3dDevice->CreateVertexShader(
        vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(),
        nullptr, vs_.GetAddressOf()),
        "SpritePipeline::Initialize: CreateVertexShader failed.");

    ThrowIfFailed(d3dDevice->CreatePixelShader(
        psBlob->GetBufferPointer(), psBlob->GetBufferSize(),
        nullptr, ps_.GetAddressOf()),
        "SpritePipeline::Initialize: CreatePixelShader failed.");

    D3D11_INPUT_ELEMENT_DESC layoutDesc[] = {
        { "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 8, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };

    ThrowIfFailed(d3dDevice->CreateInputLayout(
        layoutDesc, ARRAYSIZE(layoutDesc),
        vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(),
        inputLayout_.GetAddressOf()),
        "SpritePipeline::Initialize: CreateInputLayout failed.");

    D3D11_SAMPLER_DESC sampDesc{};
    sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
    sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
    sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
    sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

    ThrowIfFailed(d3dDevice->CreateSamplerState(&sampDesc, samp_.GetAddressOf()),
        "SpritePipeline::Initialize: CreateSamplerState failed.");

    D3D11_BLEND_DESC blendDesc{};
    blendDesc.AlphaToCoverageEnable  = FALSE;
    blendDesc.IndependentBlendEnable = FALSE;
    
    auto& rtBlend = blendDesc.RenderTarget[0];
    rtBlend.BlendEnable           = TRUE;
    rtBlend.SrcBlend              = D3D11_BLEND_SRC_ALPHA;
    rtBlend.DestBlend             = D3D11_BLEND_INV_SRC_ALPHA;
    rtBlend.BlendOp               = D3D11_BLEND_OP_ADD;
    rtBlend.SrcBlendAlpha         = D3D11_BLEND_ONE;
    rtBlend.DestBlendAlpha        = D3D11_BLEND_INV_SRC_ALPHA;
    rtBlend.BlendOpAlpha          = D3D11_BLEND_OP_ADD;
    rtBlend.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

    ThrowIfFailed(d3dDevice->CreateBlendState(&blendDesc, blend_.GetAddressOf()),
        "SpritePipeline::Initialize: CreateBlendState failed.");

    D3D11_RASTERIZER_DESC rastDesc{};
    rastDesc.FillMode = D3D11_FILL_SOLID;
    rastDesc.CullMode = D3D11_CULL_NONE;
    rastDesc.DepthClipEnable = TRUE;

    ThrowIfFailed(d3dDevice->CreateRasterizerState(&rastDesc, rast_.GetAddressOf()),
        "SpritePipeline::Initialize: CreateRasterizerState failed.");

    D3D11_BUFFER_DESC cbDesc{};
    cbDesc.ByteWidth = sizeof(CBFrameData);
    cbDesc.Usage = D3D11_USAGE_DYNAMIC;
    cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

    ThrowIfFailed(d3dDevice->CreateBuffer(&cbDesc, nullptr, cbFrame_.GetAddressOf()),
        "SpritePipeline::Initialize: CreateBuffer for cbFrame_ failed.");
}

void SpritePipeline::Bind(ID3D11DeviceContext* context) {
    context->IASetInputLayout(inputLayout_.Get());
    context->VSSetShader(vs_.Get(), nullptr, 0);
    context->PSSetShader(ps_.Get(), nullptr, 0);

    context->RSSetState(rast_.Get());

    float blendFactor[4] = {0.0f, 0.0f, 0.0f, 0.0f};
    context->OMSetBlendState(blend_.Get(), blendFactor, 0xFFFFFFFF);

    ID3D11SamplerState* samplers[] = { samp_.Get() };
    context->PSSetSamplers(0, 1, samplers);
}

void SpritePipeline::SetFrameConstants(ID3D11DeviceContext* context, uint32_t screenW, uint32_t screenH) {
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    ThrowIfFailed(context->Map(cbFrame_.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource),
        "SpritePipeline::SetFrameConstants: Map cbFrame_ failed.");

    CBFrameData* cbData = reinterpret_cast<CBFrameData*>(mappedResource.pData);
    cbData->screenSize[0] = static_cast<float>(screenW);
    cbData->screenSize[1] = static_cast<float>(screenH);

    context->Unmap(cbFrame_.Get(), 0);

    ID3D11Buffer* cbs[] = { cbFrame_.Get() };
    context->VSSetConstantBuffers(0, 1, cbs);
}

void SpritePipeline::BindTexture(ID3D11DeviceContext* context, ID3D11ShaderResourceView* srv) {
    ID3D11ShaderResourceView* srvs[] = { srv };
    context->PSSetShaderResources(0, 1, srvs);
}

} // namespace Salt2D::Render
