// Render/Pipeline/MeshPipeline.cpp
#include "MeshPipeline.h"
#include "Render/Shader/ShaderManager.h"
#include "RHI/DX11/DX11Device.h"
#include "RHI/DX11/DX11Common.h"

using namespace DirectX;

namespace Salt2D::Render {

struct CBuffers {
    XMFLOAT4X4 worldViewProj;
};

void MeshPipeline::Initialize(const RHI::DX11::DX11Device& device, ShaderManager& shaderManager) {
    auto d3dDevice = device.GetDevice();

    auto vsBlob = shaderManager.LoadShader("mesh_vs.cso");
    auto psBlob = shaderManager.LoadShader("mesh_ps.cso");
    if (!vsBlob || !psBlob) throw std::runtime_error("Failed to load MeshPipeline shaders.");

    ThrowIfFailed(d3dDevice->CreateVertexShader(
        vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(),
        nullptr, vs_.GetAddressOf()), 
        "Failed to create MeshPipeline vertex shader.");

    ThrowIfFailed(d3dDevice->CreatePixelShader(
        psBlob->GetBufferPointer(), psBlob->GetBufferSize(),
        nullptr, ps_.GetAddressOf()), 
        "Failed to create MeshPipeline pixel shader.");

    D3D11_INPUT_ELEMENT_DESC layout[] = {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(VertexCPU, pos), D3D11_INPUT_PER_VERTEX_DATA, 0 },
        // { "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(VertexCPU, normal), D3D11_INPUT_PER_VERTEX_DATA, 0 },
        // { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, offsetof(VertexCPU, uv), D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, offsetof(VertexCPU, color), D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };

    ThrowIfFailed(d3dDevice->CreateInputLayout(
        layout, ARRAYSIZE(layout),
        vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(),
        inputLayout_.GetAddressOf()),
        "Failed to create MeshPipeline input layout.");

    D3D11_RASTERIZER_DESC rastDesc{};
    rastDesc.FillMode = D3D11_FILL_SOLID;
    rastDesc.CullMode = D3D11_CULL_BACK;
    rastDesc.DepthClipEnable = TRUE;

    ThrowIfFailed(d3dDevice->CreateRasterizerState(&rastDesc, rastState_.GetAddressOf()),
        "Failed to create MeshPipeline rasterizer state.");

    D3D11_BUFFER_DESC cbDesc{};
    cbDesc.ByteWidth = sizeof(CBuffers);
    cbDesc.Usage = D3D11_USAGE_DYNAMIC;
    cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    ThrowIfFailed(d3dDevice->CreateBuffer(&cbDesc, nullptr, constantBuffer_.GetAddressOf()),
        "Failed to create MeshPipeline constant buffer.");
}

void MeshPipeline::Bind(ID3D11DeviceContext* context) {
    context->IASetInputLayout(inputLayout_.Get());
    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    context->RSSetState(rastState_.Get());

    context->VSSetShader(vs_.Get(), nullptr, 0);
    context->PSSetShader(ps_.Get(), nullptr, 0);
}

void MeshPipeline::SetConstants(ID3D11DeviceContext* context, const DirectX::XMMATRIX& worldViewProj) {
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    ThrowIfFailed(context->Map(constantBuffer_.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource),
        "Failed to map MeshPipeline constant buffer.");

    CBuffers* cbData = reinterpret_cast<CBuffers*>(mappedResource.pData);
    XMMATRIX wvpTransposed = DirectX::XMMatrixTranspose(worldViewProj);
    DirectX::XMStoreFloat4x4(&cbData->worldViewProj, wvpTransposed);

    context->Unmap(constantBuffer_.Get(), 0);

    ID3D11Buffer* cbuffers[] = { constantBuffer_.Get() };
    context->VSSetConstantBuffers(0, 1, cbuffers);
}

} // namespace Salt2D::Render
