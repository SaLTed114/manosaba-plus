//Render/Pipeline/SpritePipeline.h
#ifndef RENDER_PIPELINE_SPRITEPIPELINE_H
#define RENDER_PIPELINE_SPRITEPIPELINE_H

#include <cstdint>
#include <wrl/client.h>
#include <d3d11.h>

namespace Salt2D::RHI::DX11 {
    class DX11Device;
    class DX11SwapChain;
} // namespace Salt2D::RHI::DX11

namespace Salt2D::Render {

class ShaderManager;

class SpritePipeline {
public:
    void Initialize(const RHI::DX11::DX11Device& device, ShaderManager& shaderManager);
    
    void Bind(ID3D11DeviceContext* context);
    void SetFrameConstants(ID3D11DeviceContext* context, uint32_t screenW,  uint32_t screenH);
    void BindTexture(ID3D11DeviceContext* context, ID3D11ShaderResourceView* srv);

private:
    Microsoft::WRL::ComPtr<ID3D11VertexShader> vs_;
    Microsoft::WRL::ComPtr<ID3D11PixelShader>  ps_;
    Microsoft::WRL::ComPtr<ID3D11InputLayout>  inputLayout_;
    
    Microsoft::WRL::ComPtr<ID3D11SamplerState>    samp_;
    Microsoft::WRL::ComPtr<ID3D11BlendState>      blend_;
    Microsoft::WRL::ComPtr<ID3D11RasterizerState> rast_;

    Microsoft::WRL::ComPtr<ID3D11Buffer> cbFrame_;
};

} // namespace Salt2D::Render

#endif // RENDER_PIPELINE_SPRITEPIPELINE_H
