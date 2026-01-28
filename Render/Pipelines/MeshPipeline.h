// Render/Pipelines/MeshPipeline.h
#ifndef RENDER_PIPELINES_MESHPIPELINE_H
#define RENDER_PIPELINES_MESHPIPELINE_H

#include <wrl/client.h>
#include <d3d11.h>
#include <DirectXMath.h>

namespace Salt2D::RHI::DX11 {
    class DX11Device;
} // namespace Salt2D::RHI::DX11

namespace Salt2D::Render {

class ShaderManager;

class MeshPipeline {
public:
    struct VertexCPU {
        DirectX::XMFLOAT3 pos;
        // DirectX::XMFLOAT3 normal;
        // DirectX::XMFLOAT2 uv;
        DirectX::XMFLOAT4 color;
    };

    void Initialize(const RHI::DX11::DX11Device& device, ShaderManager& shaderManager);
    void Bind(ID3D11DeviceContext* context);

    void SetConstants(ID3D11DeviceContext* context, const DirectX::XMMATRIX& worldViewProj);

private:
    Microsoft::WRL::ComPtr<ID3D11VertexShader> vs_;
    Microsoft::WRL::ComPtr<ID3D11PixelShader>  ps_;
    Microsoft::WRL::ComPtr<ID3D11InputLayout>  inputLayout_;
    Microsoft::WRL::ComPtr<ID3D11Buffer>       constantBuffer_;
    Microsoft::WRL::ComPtr<ID3D11RasterizerState> rastState_;
};

} // namespace Salt2D::Render

#endif // RENDER_PIPELINES_MESHPIPELINE_H
