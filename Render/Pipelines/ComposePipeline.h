// Render/Pipelines/ComposePipeline.h
#ifndef RENDER_PIPELINES_COMPOSEPIPELINE_H
#define RENDER_PIPELINES_COMPOSEPIPELINE_H

#include <wrl/client.h>
#include <d3d11.h>

namespace Salt2D::RHI::DX11 {
    class DX11Device;
} // namespace Salt2D::RHI::DX11

namespace Salt2D::Render {

class ShaderManager;

class ComposePipeline {
public:
    void Initialize(const RHI::DX11::DX11Device& device, ShaderManager& shaderManager);

    void Draw(const RHI::DX11::DX11Device& device, ID3D11ShaderResourceView* sceneSRV);

private:
    Microsoft::WRL::ComPtr<ID3D11VertexShader> vs_;
    Microsoft::WRL::ComPtr<ID3D11PixelShader>  ps_;
    Microsoft::WRL::ComPtr<ID3D11SamplerState> samp_;
    Microsoft::WRL::ComPtr<ID3D11RasterizerState> rast_;
};

} // namespace Salt2D::Render

#endif // RENDER_PIPELINES_COMPOSEPIPELINE_H
