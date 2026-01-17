// Render/Primitives/TriangleDemo.cpp
#include "TriangleDemo.h"
#include "Render/Shader/ShaderManager.h"

#include <stdexcept>

namespace Salt2D::Render {

void TriangleDemo::Initialize(ID3D11Device* device, ShaderManager& shaderManager) {
    // 1) Load precompiled shaders
    auto vsBlob = shaderManager.LoadShader("triangle_vs.cso");
    auto psBlob = shaderManager.LoadShader("triangle_ps.cso");

    if (!vsBlob || !psBlob) {
        throw std::runtime_error("TriangleDemo: shader blob is null.");
    }

    // 2) Create shaders
    HRESULT hr = device->CreateVertexShader(
        vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(),
        nullptr, vs_.GetAddressOf());
    if (FAILED(hr)) throw std::runtime_error("CreateVertexShader failed.");

    hr = device->CreatePixelShader(
        psBlob->GetBufferPointer(), psBlob->GetBufferSize(),
        nullptr, ps_.GetAddressOf());
    if (FAILED(hr)) throw std::runtime_error("CreatePixelShader failed.");

    // 3) Input layout: POSITION(float3) + COLOR(float3)
    D3D11_INPUT_ELEMENT_DESC layout[] = {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,  0,
          D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR",    0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12,
          D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };

    hr = device->CreateInputLayout(
        layout, (UINT)std::size(layout),
        vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(),
        inputLayout_.GetAddressOf());
    if (FAILED(hr)) throw std::runtime_error("CreateInputLayout failed.");

    // 4) Hard-coded vertex buffer: pos(x,y,z) + color(r,g,b)
    const float vertices[] = {
        // x,    y,    z,     r,   g,   b
        -0.6f, -0.6f, 0.0f,  1.0f,0.0f,0.0f,
         0.0f,  0.7f, 0.0f,  0.0f,1.0f,0.0f,
         0.6f, -0.6f, 0.0f,  0.0f,0.0f,1.0f,
    };

    stride_ = 6 * sizeof(float);
    offset_ = 0;

    D3D11_BUFFER_DESC bd{};
    bd.ByteWidth = (UINT)sizeof(vertices);
    bd.Usage = D3D11_USAGE_IMMUTABLE;
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;

    D3D11_SUBRESOURCE_DATA init{};
    init.pSysMem = vertices;

    hr = device->CreateBuffer(&bd, &init, vb_.GetAddressOf());
    if (FAILED(hr)) throw std::runtime_error("CreateBuffer(vertex buffer) failed.");
}

void TriangleDemo::Draw(ID3D11DeviceContext* ctx, ID3D11RenderTargetView* rtv) {
    if (!ctx || !rtv) return;

    // Bind RT
    ID3D11RenderTargetView* rtvs[] = { rtv };
    ctx->OMSetRenderTargets(1, rtvs, nullptr);

    // IA
    ID3D11Buffer* vbs[] = { vb_.Get() };
    ctx->IASetInputLayout(inputLayout_.Get());
    ctx->IASetVertexBuffers(0, 1, vbs, &stride_, &offset_);
    ctx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    // Shaders
    ctx->VSSetShader(vs_.Get(), nullptr, 0);
    ctx->PSSetShader(ps_.Get(), nullptr, 0);

    // Draw
    ctx->Draw(3, 0);
}

} // namespace Salt2D::Render
