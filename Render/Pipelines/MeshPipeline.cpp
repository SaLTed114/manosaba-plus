// Render/Pipelines/MeshPipeline.cpp
#include "MeshPipeline.h"
#include "Render/Shader/ShaderManager.h"
#include "RHI/DX11/DX11Device.h"
#include "RHI/DX11/DX11Common.h"

using namespace DirectX;

namespace Salt2D::Render {

struct CBuffers {
    XMFLOAT4X4 worldViewProj;     // 64 bytes
    XMFLOAT4X4 world;              // 64 bytes
    XMFLOAT4X4 worldInvTranspose;  // 64 bytes
    XMFLOAT4   lightPosWS;         // 16 bytes (xyz = position, w unused)
    XMFLOAT4   lightColorAndAmbient; // 16 bytes (rgb = color, w = ambient)
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
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, offsetof(VertexCPU, pos), D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT,    0, offsetof(VertexCPU, normal), D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,       0, offsetof(VertexCPU, uv), D3D11_INPUT_PER_VERTEX_DATA, 0 },
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
    // Use default world matrix (identity) and default lighting
    XMMATRIX world = XMMatrixIdentity();
    SetConstants(context, world, worldViewProj);
}

void MeshPipeline::SetConstants(
    ID3D11DeviceContext* context,
    const DirectX::XMMATRIX& world,
    const DirectX::XMMATRIX& worldViewProj
) {
    // Use default light position
    SetConstants(context, world, worldViewProj, XMFLOAT3(0.0f, 10.0f, 0.0f));
}

void MeshPipeline::SetConstants(
    ID3D11DeviceContext* context,
    const DirectX::XMMATRIX& world,
    const DirectX::XMMATRIX& worldViewProj,
    const DirectX::XMFLOAT3& lightPos
) {
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    ThrowIfFailed(context->Map(constantBuffer_.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource),
        "Failed to map MeshPipeline constant buffer.");

    CBuffers* cbData = reinterpret_cast<CBuffers*>(mappedResource.pData);
    
    // Store transposed matrices (HLSL uses column-major)
    XMStoreFloat4x4(&cbData->worldViewProj, XMMatrixTranspose(worldViewProj));
    XMStoreFloat4x4(&cbData->world, XMMatrixTranspose(world));
    
    // Calculate world inverse transpose for normal transformation
    XMVECTOR det;
    XMMATRIX worldInv = XMMatrixInverse(&det, world);
    XMMATRIX worldInvTranspose = XMMatrixTranspose(worldInv);
    XMStoreFloat4x4(&cbData->worldInvTranspose, XMMatrixTranspose(worldInvTranspose));
    
    // Lighting parameters (point light with custom position)
    cbData->lightPosWS = XMFLOAT4(lightPos.x, lightPos.y, lightPos.z, 0.0f);
    cbData->lightColorAndAmbient = XMFLOAT4(1.0f, 1.0f, 1.0f, 0.3f);  // rgb = color, w = ambient

    context->Unmap(constantBuffer_.Get(), 0);

    ID3D11Buffer* cbuffers[] = { constantBuffer_.Get() };
    context->VSSetConstantBuffers(0, 1, cbuffers);
    context->PSSetConstantBuffers(0, 1, cbuffers);
}

} // namespace Salt2D::Render
