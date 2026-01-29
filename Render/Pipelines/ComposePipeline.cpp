// Render/Pipeline/ComposePipeline.cpp
#include "ComposePipeline.h"
#include "Render/Shader/ShaderManager.h"
#include "RHI/DX11/DX11Common.h"
#include "RHI/DX11/DX11Device.h"

#include <stdexcept>

namespace Salt2D::Render {

void ComposePipeline::Initialize(const RHI::DX11::DX11Device& device, ShaderManager& shaderManager) {
    auto d3dDevice = device.GetDevice();

    auto vsBlob = shaderManager.LoadShader("compose_vs.cso");
    auto psBlob = shaderManager.LoadShader("compose_ps.cso");
    if (!vsBlob || !psBlob) throw std::runtime_error("ComposePipeline::Initialize: Failed to load shaders.");

    ThrowIfFailed(d3dDevice->CreateVertexShader(
        vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(),
        nullptr, vs_.GetAddressOf()), 
        "ComposePipeline::Initialize: CreateVertexShader failed.");

    ThrowIfFailed(d3dDevice->CreatePixelShader(
        psBlob->GetBufferPointer(), psBlob->GetBufferSize(),
        nullptr, ps_.GetAddressOf()), 
        "ComposePipeline::Initialize: CreatePixelShader failed.");

    D3D11_SAMPLER_DESC sampDesc{};
    sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
    sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
    sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
    sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

    ThrowIfFailed(d3dDevice->CreateSamplerState(&sampDesc, samp_.GetAddressOf()),
        "ComposePipeline::Initialize: CreateSamplerState failed.");

    D3D11_RASTERIZER_DESC rastDesc{};
    rastDesc.FillMode = D3D11_FILL_SOLID;
    rastDesc.CullMode = D3D11_CULL_NONE;
    rastDesc.DepthClipEnable = TRUE;

    ThrowIfFailed(d3dDevice->CreateRasterizerState(&rastDesc, rast_.GetAddressOf()),
        "ComposePipeline::Initialize: CreateRasterizerState failed.");
}

void ComposePipeline::Bind(ID3D11DeviceContext* context) {
    context->IASetInputLayout(nullptr);
    context->IASetVertexBuffers(0, 0, nullptr, nullptr, nullptr);
    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    context->RSSetState(rast_.Get());

    context->VSSetShader(vs_.Get(), nullptr, 0);
    context->PSSetShader(ps_.Get(), nullptr, 0);

    ID3D11SamplerState* samplers[] = { samp_.Get() };
    context->PSSetSamplers(0, 1, samplers);
}

} // namespace Salt2D::Render
