// Render/Pipelines/CardPipeline.cpp
#include "CardPipeline.h"
#include "RHI/DX11/DX11Device.h"
#include "RHI/DX11/DX11Common.h"
#include "Render/Shader/ShaderManager.h"

#include <stdexcept>

namespace Salt2D::Render {

using namespace DirectX;

struct CBFrameData {
    XMFLOAT4X4 viewProj;
};

struct alignas(16) CBCardData {
    XMFLOAT3 position;
    float    yaw;
    XMFLOAT2 size;
    float    alphaCut;
    float    pad;
};

void CardPipeline::Initialize(const RHI::DX11::DX11Device& device, ShaderManager& shaderManager) {
    auto d3dDevice = device.GetDevice();

    auto vsBlob = shaderManager.LoadShader("card_vs.cso");
    auto psBlob = shaderManager.LoadShader("card_ps.cso");

    ThrowIfFailed(d3dDevice->CreateVertexShader(
        vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(),
        nullptr, vs_.GetAddressOf()),
        "CardPipeline::Initialize: CreateVertexShader failed.");

    ThrowIfFailed(d3dDevice->CreatePixelShader(
        psBlob->GetBufferPointer(), psBlob->GetBufferSize(),
        nullptr, ps_.GetAddressOf()),
        "CardPipeline::Initialize: CreatePixelShader failed.");

    D3D11_INPUT_ELEMENT_DESC inputDesc[] = {
        { "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 8, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };

    ThrowIfFailed(d3dDevice->CreateInputLayout(
        inputDesc, ARRAYSIZE(inputDesc),
        vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(),
        inputLayout_.GetAddressOf()),
        "CardPipeline::Initialize: CreateInputLayout failed.");

    D3D11_SAMPLER_DESC sampDesc{};
    sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
    sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
    sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
    sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

    ThrowIfFailed(d3dDevice->CreateSamplerState(&sampDesc, samp_.GetAddressOf()),
        "CardPipeline::Initialize: CreateSamplerState failed.");

    D3D11_RASTERIZER_DESC rastDesc{};
    rastDesc.FillMode = D3D11_FILL_SOLID;
    rastDesc.CullMode = D3D11_CULL_NONE;
    rastDesc.DepthClipEnable = TRUE;
    rastDesc.FrontCounterClockwise = TRUE;

    ThrowIfFailed(d3dDevice->CreateRasterizerState(&rastDesc, rast_.GetAddressOf()),
        "CardPipeline::Initialize: CreateRasterizerState failed.");

    D3D11_BUFFER_DESC cbFrameDesc{};
    cbFrameDesc.ByteWidth = sizeof(CBFrameData);
    cbFrameDesc.Usage = D3D11_USAGE_DYNAMIC;
    cbFrameDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    cbFrameDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

    ThrowIfFailed(d3dDevice->CreateBuffer(&cbFrameDesc, nullptr, cbFrame_.GetAddressOf()),
        "CardPipeline::Initialize: CreateBuffer for cbFrame_ failed.");
    
    D3D11_BUFFER_DESC cbCardDesc{};
    cbCardDesc.ByteWidth = sizeof(CBCardData);
    cbCardDesc.Usage = D3D11_USAGE_DYNAMIC;
    cbCardDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    cbCardDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

    ThrowIfFailed(d3dDevice->CreateBuffer(&cbCardDesc, nullptr, cbCard_.GetAddressOf()),
        "CardPipeline::Initialize: CreateBuffer for cbCard_ failed.");
}

void CardPipeline::Bind(ID3D11DeviceContext* context) {
    context->IASetInputLayout(inputLayout_.Get());
    context->VSSetShader(vs_.Get(), nullptr, 0);
    context->PSSetShader(ps_.Get(), nullptr, 0);

    context->RSSetState(rast_.Get());

    ID3D11SamplerState* samplers[] = { samp_.Get() };
    context->PSSetSamplers(0, 1, samplers);
}

void CardPipeline::SetFrameConstants(ID3D11DeviceContext* context, const XMMATRIX& viewProj) {
    D3D11_MAPPED_SUBRESOURCE mapped{};
    ThrowIfFailed(context->Map(cbFrame_.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped),
        "CardPipeline::SetFrameConstants: Map failed.");

    CBFrameData* data = reinterpret_cast<CBFrameData*>(mapped.pData);
    XMStoreFloat4x4(&data->viewProj, XMMatrixTranspose(viewProj));

    context->Unmap(cbFrame_.Get(), 0);

    ID3D11Buffer* cbs[] = { cbFrame_.Get() };
    context->VSSetConstantBuffers(0, 1, cbs);
}

void CardPipeline::SetCardConstants(
    ID3D11DeviceContext* context,
    const XMFLOAT3& pos, float yaw,
    const XMFLOAT2& size, float alphaCut
) {
    D3D11_MAPPED_SUBRESOURCE mapped{};
    ThrowIfFailed(context->Map(cbCard_.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped),
        "CardPipeline::SetCardConstants: Map failed.");

    CBCardData* data = reinterpret_cast<CBCardData*>(mapped.pData);
    data->position = pos;
    data->yaw = yaw;
    data->size = size;
    data->alphaCut = alphaCut;

    context->Unmap(cbCard_.Get(), 0);

    ID3D11Buffer* cbs[] = { cbCard_.Get() };
    context->VSSetConstantBuffers(1, 1, cbs);
    context->PSSetConstantBuffers(1, 1, cbs);
}

void CardPipeline::BindTexture(ID3D11DeviceContext* context, ID3D11ShaderResourceView* srv) {
    ID3D11ShaderResourceView* srvs[] = { srv };
    context->PSSetShaderResources(0, 1, srvs);
}

} // namespace Salt2D::Render
