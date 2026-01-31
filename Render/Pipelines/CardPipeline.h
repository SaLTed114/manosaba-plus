// Render/Pipelines/CardPipeline.h
#ifndef RENDER_PIPELINES_CARDPIPELINE_H
#define RENDER_PIPELINES_CARDPIPELINE_H

#include <wrl/client.h>
#include <d3d11.h>
#include <DirectXMath.h>

namespace Salt2D::RHI::DX11 {
    class DX11Device;
} // namespace Salt2D::RHI::DX11

namespace Salt2D::Render {

class ShaderManager;

class CardPipeline {
public:
    void Initialize(const RHI::DX11::DX11Device& device, ShaderManager& shaderManager);
    void Bind(ID3D11DeviceContext* context);

    void SetFrameConstants(ID3D11DeviceContext* context, const DirectX::XMMATRIX& viewProj);
    void SetCardConstants(ID3D11DeviceContext* context,
        const DirectX::XMFLOAT3& pos, float yaw,
        const DirectX::XMFLOAT2& size, float alphaCut);

    void BindTexture(ID3D11DeviceContext* context, ID3D11ShaderResourceView* srv);

private:
    Microsoft::WRL::ComPtr<ID3D11VertexShader> vs_;
    Microsoft::WRL::ComPtr<ID3D11PixelShader>  ps_;
    Microsoft::WRL::ComPtr<ID3D11InputLayout>  inputLayout_;

    Microsoft::WRL::ComPtr<ID3D11SamplerState>    samp_;
    Microsoft::WRL::ComPtr<ID3D11RasterizerState> rast_;

    Microsoft::WRL::ComPtr<ID3D11Buffer> cbFrame_;
    Microsoft::WRL::ComPtr<ID3D11Buffer> cbCard_;
};

} // namespace Salt2D::Render

#endif // RENDER_PIPELINES_CARDPIPELINE_H
